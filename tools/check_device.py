"""Read-only H08 HTTP checks against the URL explicitly supplied by a learner."""
from __future__ import annotations

import argparse
import json
import time
from urllib.error import HTTPError
from urllib.parse import urlsplit
from urllib.request import urlopen


def check_device(url, timeout=3):
    parts = urlsplit(url)
    if parts.scheme not in ("http", "https") or not parts.hostname or parts.username:
        raise ValueError("提供设备的 http(s) 地址，不包含密码")
    base = url.rstrip("/")
    uptimes = []
    for _ in range(2):
        with urlopen(base + "/api/status", timeout=timeout) as response:
            if response.status != 200:
                raise ValueError(f"状态 API 预期 200，实际 {response.status}")
            if response.headers.get_content_type() != "application/json":
                raise ValueError("状态 API Content-Type 不是 application/json")
            body = response.read(65537)
            if len(body) > 65536:
                raise ValueError("状态响应超过 64 KiB")
            payload = json.loads(body)
        if not isinstance(payload, dict):
            raise ValueError("状态必须是 JSON object")
        uptime = payload.get("uptime_ms")
        if type(uptime) is not int or uptime < 0:
            raise ValueError("uptime_ms 必须为非负整数")
        uptimes.append(uptime)
        time.sleep(0.05)
    if uptimes[1] <= uptimes[0]:
        raise ValueError("uptime_ms 没有增长；检查缓存、复位或旧响应")
    try:
        with urlopen(base + "/api/does-not-exist", timeout=timeout) as response:
            raise ValueError(f"未知路径预期 404，实际 {response.status}")
    except HTTPError as exc:
        try:
            if exc.code != 404:
                raise ValueError(f"未知路径预期 404，实际 {exc.code}") from exc
        finally:
            exc.close()
    return uptimes


def main():
    parser = argparse.ArgumentParser(description="只读检查真实设备 HTTP，不控制 GPIO 或更新固件")
    parser.add_argument("--url", required=True)
    parser.add_argument("--timeout", type=float, default=3)
    args = parser.parse_args()
    if not 0 < args.timeout <= 60:
        parser.error("timeout 要在 (0, 60] 秒内")
    try:
        uptimes = check_device(args.url, args.timeout)
    except (OSError, ValueError) as exc:
        parser.exit(1, f"✗ HTTP 检查失败：{exc}\n")
    print(f"✓ 状态 JSON、增长的 uptime_ms={uptimes}、未知路径 404")
    print("证据仅为提供 URL 的 HTTP 往返；LED/接线/无线射频需另行检查。")


if __name__ == "__main__":
    main()
