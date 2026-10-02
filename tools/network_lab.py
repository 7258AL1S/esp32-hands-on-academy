"""Real localhost networking labs used by Lessons 13–18.

The notebook kernel owns only the visual panel and test clients. Every request
shown by the panel travels through a real socket to the C++ code in the current
cell. The helpers deliberately bind only 127.0.0.1.
"""
from __future__ import annotations

import base64
import atexit
import contextlib
import html as html_module
import http.client
import os
from pathlib import Path
import queue
import re
import secrets
import shutil
import socket
import subprocess
import threading
import time
from dataclasses import dataclass

import ipywidgets as widgets
from IPython import get_ipython
from IPython.core.magic import register_cell_magic

from notebook_theme import FAILURE, SUCCESS, check_card, controls, html, panel

ROOT = Path(__file__).resolve().parents[1]
LOOPBACK = "127.0.0.1"
READY = re.compile(r"^ACADEMY_READY\s+(tcp|udp|http|ws)\s+(\d+)$")


def _read_exact(conn: socket.socket, length: int) -> bytes:
    output = bytearray()
    while len(output) < length:
        chunk = conn.recv(length - len(output))
        if not chunk:
            raise RuntimeError("连接在协议帧结束前关闭")
        output.extend(chunk)
    return bytes(output)


def _recv_until(conn: socket.socket, marker: bytes, max_bytes: int = 65536) -> bytes:
    output = bytearray()
    while marker not in output and len(output) < max_bytes:
        chunk = conn.recv(min(4096, max_bytes - len(output)))
        if not chunk:
            break
        output.extend(chunk)
    return bytes(output)


class _LineFixture:
    """A real local endpoint for the DNS/connection lab, never a fake response."""

    def __init__(self):
        self.listener = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.listener.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.listener.bind((LOOPBACK, 0))
        self.listener.listen(4)
        self.listener.settimeout(.15)
        self.port = self.listener.getsockname()[1]
        self.stop_event = threading.Event()
        self.thread = threading.Thread(target=self._serve, daemon=True)
        self.thread.start()

    def _serve(self):
        while not self.stop_event.is_set():
            try:
                conn, _ = self.listener.accept()
            except socket.timeout:
                continue
            with contextlib.closing(conn):
                conn.settimeout(1)
                request = _recv_until(conn, b"\n")
                if request == b"PING\n":
                    conn.sendall(b"PONG\n")
                else:
                    conn.sendall(b"ERR\n")

    def close(self):
        self.stop_event.set()
        with contextlib.suppress(OSError):
            self.listener.close()
        self.thread.join(timeout=1)


class MiniBroker:
    """MQTT 3.1.1 QoS 0 fixture for one Lesson 18 client execution.

    It accepts only CONNECT, SUBSCRIBE and PUBLISH, records actual packets, and
    sends one command after subscription. It is intentionally a course fixture,
    not a replacement for Mosquitto.
    """

    def __init__(self):
        self.listener = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.listener.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.listener.bind((LOOPBACK, 0))
        self.listener.listen(1)
        self.listener.settimeout(.15)
        self.port = self.listener.getsockname()[1]
        self.messages: list[tuple[str, str]] = []
        self.error: str | None = None
        self.done = threading.Event()
        self._stop = threading.Event()
        self.thread = threading.Thread(target=self._serve, daemon=True)
        self.thread.start()

    @staticmethod
    def _read_packet(conn: socket.socket) -> tuple[int, bytes]:
        first = _read_exact(conn, 1)[0]
        value = 0
        multiplier = 1
        for _ in range(4):
            byte = _read_exact(conn, 1)[0]
            value += (byte & 127) * multiplier
            if not byte & 128:
                return first, _read_exact(conn, value)
            multiplier *= 128
        raise RuntimeError("无效 MQTT Remaining Length")

    @staticmethod
    def _packet(first: int, body: bytes) -> bytes:
        length = len(body)
        encoded = bytearray()
        while True:
            byte = length % 128
            length //= 128
            encoded.append(byte | (128 if length else 0))
            if not length:
                return bytes([first]) + bytes(encoded) + body

    @staticmethod
    def _string(body: bytes, offset: int = 0) -> tuple[str, int]:
        length = (body[offset] << 8) | body[offset + 1]
        offset += 2
        return body[offset:offset + length].decode("utf-8"), offset + length

    @classmethod
    def _publish(cls, topic: str, payload: str) -> bytes:
        topic_bytes = topic.encode()
        body = len(topic_bytes).to_bytes(2, "big") + topic_bytes + payload.encode()
        return cls._packet(0x30, body)

    def _serve(self):
        try:
            while not self._stop.is_set():
                try:
                    conn, _ = self.listener.accept()
                except socket.timeout:
                    continue
                with contextlib.closing(conn):
                    conn.settimeout(2)
                    first, _ = self._read_packet(conn)
                    if first != 0x10:
                        raise RuntimeError("第一个 MQTT packet 必须是 CONNECT")
                    conn.sendall(self._packet(0x20, b"\x00\x00"))
                    subscribed = False
                    deadline = time.monotonic() + 2
                    while time.monotonic() < deadline:
                        first, body = self._read_packet(conn)
                        if first == 0x82:
                            packet_id = body[:2]
                            conn.sendall(self._packet(0x90, packet_id + b"\x00"))
                            subscribed = True
                            conn.sendall(self._publish("desk/reminder/cmd", "toggle"))
                        elif first & 0xf0 == 0x30:
                            topic, offset = self._string(body)
                            self.messages.append((topic, body[offset:].decode("utf-8")))
                        if subscribed and ("desk/reminder/state", "on") in self.messages:
                            self.done.set()
                            return
        except Exception as exc:  # fixture diagnostics become visible in the panel
            self.error = str(exc)
        finally:
            self.done.set()

    def close(self):
        self._stop.set()
        with contextlib.suppress(OSError):
            self.listener.close()
        self.thread.join(timeout=1)


@dataclass
class Result:
    request: str
    response: str
    ok: bool


class NetworkLab:
    def __init__(self, name: str, kind: str, executable: Path):
        self.name = name
        self.kind = kind
        self.executable = executable
        self.process: subprocess.Popen[str] | None = None
        self.lines: queue.Queue[str] = queue.Queue()
        self.reader: threading.Thread | None = None
        self.port: int | None = None
        self.fixture: _LineFixture | None = None
        self.broker: MiniBroker | None = None

    def _reader(self):
        assert self.process is not None and self.process.stdout is not None
        for line in self.process.stdout:
            self.lines.put(line.rstrip("\n"))

    def _launch(self, env: dict[str, str]) -> None:
        self.close()
        merged = os.environ.copy()
        merged.update(env)
        self.process = subprocess.Popen(
            [str(self.executable)], stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
            text=True, bufsize=1, env=merged,
        )
        self.reader = threading.Thread(target=self._reader, daemon=True)
        self.reader.start()

    def start(self) -> int:
        if self.kind == "dns":
            if self.fixture is None:
                self.fixture = _LineFixture()
            return self.fixture.port
        if self.kind == "mqtt":
            return 0
        if self.process is not None and self.process.poll() is None and self.port is not None:
            return self.port
        self._launch({"ACADEMY_PORT": "0"})
        deadline = time.monotonic() + 3
        captured: list[str] = []
        while time.monotonic() < deadline:
            try:
                line = self.lines.get(timeout=.1)
            except queue.Empty:
                if self.process and self.process.poll() is not None:
                    break
                continue
            captured.append(line)
            match = READY.match(line)
            if match and match.group(1) == self.kind:
                self.port = int(match.group(2))
                return self.port
        self.close()
        raise RuntimeError("C++ 服务没有报告临时端口：" + " | ".join(captured))

    def close(self):
        if self.process is not None:
            if self.process.poll() is None:
                self.process.terminate()
                try:
                    self.process.wait(timeout=1)
                except subprocess.TimeoutExpired:
                    self.process.kill()
                    self.process.wait(timeout=1)
            self.process = None
        if self.fixture is not None:
            self.fixture.close()
            self.fixture = None
        if self.broker is not None:
            self.broker.close()
            self.broker = None
        self.port = None

    def restart(self):
        self.close()
        return self.start()

    def tcp(self, message: str) -> Result:
        port = self.start()
        with socket.create_connection((LOOPBACK, port), timeout=1.5) as conn:
            conn.settimeout(1.5)
            conn.sendall(message.encode())
            response = _recv_until(conn, b"\n").decode("utf-8", "replace")
        return Result(message, response, bool(response))

    def udp(self, message: str) -> Result:
        port = self.start()
        with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as conn:
            conn.settimeout(1.5)
            conn.sendto(message.encode(), (LOOPBACK, port))
            response, _ = conn.recvfrom(2048)
        return Result(message, response.decode("utf-8", "replace"), bool(response))

    def http(self, method: str, path: str, body: str | None = None) -> Result:
        port = self.start()
        conn = http.client.HTTPConnection(LOOPBACK, port, timeout=1.5)
        try:
            headers = {"Content-Type": "application/json"} if body is not None else {}
            conn.request(method, path, body=body, headers=headers)
            reply = conn.getresponse()
            text = reply.read().decode("utf-8", "replace")
            response = f"HTTP {reply.status} {reply.reason}\nContent-Type: {reply.getheader('Content-Type')}\n\n{text}"
            return Result(f"{method} {path}" + (f"\n{body}" if body else ""), response, 200 <= reply.status < 300)
        finally:
            conn.close()

    def websocket(self, message: str) -> Result:
        port = self.start()
        with socket.create_connection((LOOPBACK, port), timeout=1.5) as conn:
            conn.settimeout(1.5)
            key = base64.b64encode(secrets.token_bytes(16)).decode()
            handshake = (
                "GET /live HTTP/1.1\r\n"
                f"Host: {LOOPBACK}:{port}\r\nUpgrade: websocket\r\nConnection: Upgrade\r\n"
                f"Sec-WebSocket-Key: {key}\r\nSec-WebSocket-Version: 13\r\n\r\n"
            )
            conn.sendall(handshake.encode())
            header = _recv_until(conn, b"\r\n\r\n").decode("utf-8", "replace")
            if not header.startswith("HTTP/1.1 101"):
                return Result(handshake, header, False)
            payload = message.encode()
            mask = b"\x21\x43\x65\x87"
            frame = bytearray([0x81, 0x80 | len(payload)]) + bytearray(mask)
            frame.extend(value ^ mask[i % 4] for i, value in enumerate(payload))
            conn.sendall(frame)
            first, length = _read_exact(conn, 2)
            length &= 0x7f
            if length == 126:
                length = int.from_bytes(_read_exact(conn, 2), "big")
            reply = _read_exact(conn, length).decode("utf-8", "replace")
        return Result("HTTP Upgrade → masked text frame: " + message,
                      header + "\n\nserver text frame: " + reply, first == 0x81)

    def dns(self, connected: bool) -> Result:
        port = self.start()
        env = {"ACADEMY_PORT": str(port), "ACADEMY_WIFI_CONNECTED": "1" if connected else "0"}
        completed = subprocess.run([str(self.executable)], capture_output=True, text=True, env={**os.environ, **env}, timeout=4)
        output = (completed.stdout + completed.stderr).strip()
        label = "Wi-Fi state model: CONNECTED" if connected else "Wi-Fi state model: DISCONNECTED"
        return Result(label, output, completed.returncode == 0)

    def mqtt(self) -> Result:
        self.close()
        self.broker = MiniBroker()
        env = {**os.environ, "ACADEMY_MQTT_PORT": str(self.broker.port)}
        completed = subprocess.run([str(self.executable)], capture_output=True, text=True, env=env, timeout=5)
        self.broker.done.wait(1)
        messages = ", ".join(f"{topic} = {payload}" for topic, payload in self.broker.messages) or "(no PUBLISH received)"
        fixture_error = f"\nBroker: {self.broker.error}" if self.broker.error else ""
        output = (completed.stdout + completed.stderr).strip()
        response = f"C++ client output:\n{output}\n\nBroker received:\n{messages}{fixture_error}"
        return Result("CONNECT → SUBSCRIBE desk/reminder/cmd → broker PUBLISH toggle", response,
                      completed.returncode == 0 and self.broker.error is None)

    def action(self, action: str) -> Result:
        if self.kind == "tcp":
            return self.tcp("STATUS\n" if action == "status" else "TOGGLE\n")
        if self.kind == "udp":
            return self.udp("DISCOVER")
        if self.kind == "http":
            if action == "status":
                return self.http("GET", "/api/status")
            enabled = "true" if action == "on" else "false"
            return self.http("POST", "/api/reminder", '{"enabled":' + enabled + "}")
        if self.kind == "dns":
            return self.dns(action == "connected")
        if self.kind == "ws":
            return self.websocket("status" if action == "status" else "toggle")
        if self.kind == "mqtt":
            return self.mqtt()
        raise ValueError(self.kind)

    def check(self) -> tuple[bool, list[str]]:
        self.restart() if self.kind not in {"mqtt"} else None
        try:
            if self.kind == "tcp":
                one = self.tcp("STATUS\n").response == "reminder=OFF\n"
                two = self.tcp("TOGGLE\n").response == "reminder=ON\n"
                three = self.tcp("STATUS\n").response == "reminder=ON\n"
                return all((one, two, three)), [
                    (one, "初始 STATUS 返回 reminder=OFF" if one else "初始 STATUS 应返回 reminder=OFF"),
                    (two, "TOGGLE 后立即返回 reminder=ON" if two else "TOGGLE 后应返回 reminder=ON"),
                    (three, "状态在下一个 TCP 连接中保持" if three else "状态没有在下一次连接中保持"),
                ]
            if self.kind == "udp":
                response = self.udp("DISCOVER").response
                good = response == "DESK_REMINDER|ONLINE"
                return good, [(good, "UDP datagram 保持完整边界" if good else "DISCOVER 应回复 DESK_REMINDER|ONLINE")]
            if self.kind == "http":
                one = '"reminder":false' in self.http("GET", "/api/status").response
                two = self.http("POST", "/api/reminder", '{"enabled":true}').ok
                three = '"reminder":true' in self.http("GET", "/api/status").response
                return all((one, two, three)), [
                    (one, "GET /api/status 返回 JSON 初始状态" if one else "GET /api/status 缺少初始 JSON 状态"),
                    (two, "POST /api/reminder 返回 2xx" if two else "POST /api/reminder 应成功"),
                    (three, "POST 后 GET 读到更新状态" if three else "REST 状态没有更新"),
                ]
            if self.kind == "dns":
                offline = self.dns(False).response == "WAIT_FOR_WIFI"
                online = self.dns(True).response
                good_online = "RESOLVED 127.0.0.1" in online and "PONG" in online
                return offline and good_online, [
                    (offline, "断开时不尝试连接" if offline else "断开时应报告 WAIT_FOR_WIFI"),
                    (good_online, "连接后解析 localhost 并收到 PONG" if good_online else "连接后应完成真实解析和 TCP 请求"),
                ]
            if self.kind == "ws":
                one = '"reminder":false' in self.websocket("status").response
                two = '"reminder":true' in self.websocket("toggle").response
                three = '"reminder":true' in self.websocket("status").response
                return one and two and three, [
                    (one, "HTTP Upgrade 得到 101，status frame 返回 false" if one else "WebSocket status 应返回 false"),
                    (two, "下一帧 toggle 返回 true" if two else "WebSocket toggle 应返回 true"),
                    (three, "新连接仍读到同一份提醒器状态" if three else "toggle 后的新 WebSocket 连接应读到 true"),
                ]
            if self.kind == "mqtt":
                result = self.mqtt()
                good = "state=on" in result.response and "desk/reminder/state = offline" in result.response and "desk/reminder/state = on" in result.response
                return good, [(good, "CONNECT / SUBSCRIBE / PUBLISH 都经过真实 MQTT packet" if good else "应先订阅命令，再发布 offline 和 on 状态")]
        except Exception as exc:
            return False, [(False, f"网络操作失败：{exc}")]
        return False, [(False, "未知实验类型")]


def compile_network_lab(name: str, kind: str, source: str) -> NetworkLab:
    if kind not in {"tcp", "udp", "http", "dns", "ws", "mqtt"}:
        raise ValueError("kind must be tcp, udp, http, dns, ws, or mqtt")
    if not re.fullmatch(r"[a-z][a-z0-9_-]*", name):
        raise ValueError("实验名使用小写英文，例如 tcp-guided")
    compiler = os.environ.get("CXX") or shutil.which("c++")
    if not compiler:
        raise RuntimeError("找不到 C++ compiler。已在 macOS Apple Clang 验证此网络实验支架。")
    directory = ROOT / ".build" / "network" / name
    directory.mkdir(parents=True, exist_ok=True)
    source_path = directory / "main.cpp"
    executable = directory / ("network_lab.exe" if os.name == "nt" else "network_lab")
    source_path.write_text(source, encoding="utf-8")
    command = [compiler, "-std=c++17", "-Wall", "-Wextra", "-Wpedantic", "-I", str(ROOT / "include"), str(source_path), "-o", str(executable)]
    if os.name == "nt":
        command.append("-lws2_32")
    result = subprocess.run(command, capture_output=True, text=True)
    if result.returncode:
        print(result.stdout)
        print(result.stderr)
        raise RuntimeError("C++ 构建失败；修正 compiler 诊断后重新运行本格。")
    print(f"✓ 已编译真实 {kind.upper()} 实验：{name}")
    return NetworkLab(name, kind, executable)


class NetworkPanel:
    def __init__(self, lab: NetworkLab, profile: str | None = None):
        self.lab = lab
        self.profile = profile
        self.title = html(f'<div class="academy-title">{lab.kind.upper()} 本机实验台</div>')
        self.note = html('<div class="academy-note">所有数据只在 <code>127.0.0.1</code> 间流动；控制器和协议响应来自当前 C++ 代码格。</div>')
        self.topology = html(self._topology())
        self.endpoint = html('<div class="academy-note">正在准备临时端口…</div>')
        self.trace = html('<pre style="margin:0;white-space:pre-wrap;overflow-wrap:anywhere;color:#eeeeee;background:#141414;border:1px solid #535b66;padding:10px;border-radius:4px;min-height:92px">等待操作。</pre>')
        self.status = html('')
        self.checks = html('')
        self.buttons = self._buttons()
        self.restart_button = widgets.Button(description="重启 C++ 服务", icon="refresh")
        self.restart_button.on_click(self._restart)
        self.restart_button.add_class("mod-info")
        self.check_button = widgets.Button(description="播放自动检查", icon="play")
        self.check_button.on_click(self._check)
        self.check_button.add_class("mod-warning")
        self.view = panel([self.title, self.note, self.topology, self.endpoint,
                           controls([*self.buttons, self.restart_button, self.check_button]),
                           self.trace, self.status, self.checks])
        if lab.kind not in {"mqtt", "dns"}:
            try:
                port = lab.start()
                self.endpoint.value = f'<div class="academy-note">C++ service: <code>{LOOPBACK}:{port}</code></div>'
            except Exception as exc:
                self._show_error(str(exc))

    def _topology(self) -> str:
        labels = {
            "tcp": "Notebook client → TCP stream → C++ status service",
            "udp": "Notebook client → UDP datagram → C++ discovery service",
            "http": "Notebook client → HTTP request → C++ REST API",
            "dns": "C++ client → localhost DNS → local TCP fixture",
            "ws": "Notebook client → HTTP Upgrade / WebSocket frame → C++ service",
            "mqtt": "C++ MQTT Client ⇄ local QoS 0 Broker",
        }
        return f'<div style="background:#141414;border:1px solid #535b66;border-radius:4px;padding:9px;color:#b8c2cc">{labels[self.lab.kind]}</div>'

    def _buttons(self) -> list[widgets.Button]:
        choices = {
            "tcp": [("查询 STATUS", "status"), ("发送 TOGGLE", "toggle")],
            "udp": [("发送 DISCOVER", "discover")],
            "http": [("GET 状态", "status"), ("POST 开启", "on"), ("POST 关闭", "off")],
            "dns": [("模型：Wi-Fi 断开", "disconnected"), ("模型：Wi-Fi 已连接", "connected")],
            "ws": [("发送 status frame", "status"), ("发送 toggle frame", "toggle")],
            "mqtt": [("运行 MQTT Client", "mqtt")],
        }
        output = []
        for label, action in choices[self.lab.kind]:
            button = widgets.Button(description=label)
            button.on_click(lambda _, selected=action: self._action(selected))
            output.append(button)
        return output

    def _show_error(self, message: str):
        self.status.value = check_card("✗ " + html_module.escape(message), FAILURE)

    def _show_result(self, result: Result):
        escaped_request = html_module.escape(result.request)
        escaped_response = html_module.escape(result.response)
        self.trace.value = ('<pre style="margin:0;white-space:pre-wrap;overflow-wrap:anywhere;color:#eeeeee;background:#141414;border:1px solid #535b66;padding:10px;border-radius:4px;min-height:92px">'
                            f'→ {escaped_request}\n\n← {escaped_response}</pre>')
        self.status.value = check_card("✓ 已完成真实 localhost 往返" if result.ok else "✗ 协议往返未成功", SUCCESS if result.ok else FAILURE)

    def _action(self, action: str):
        try:
            self._show_result(self.lab.action(action))
        except Exception as exc:
            self._show_error(str(exc))

    def _restart(self, _):
        try:
            port = self.lab.restart()
            self.endpoint.value = f'<div class="academy-note">C++ service: <code>{LOOPBACK}:{port}</code></div>' if port else '<div class="academy-note">下一次运行会创建本地 Broker。</div>'
            self.status.value = check_card("✓ 已回收旧进程并创建新的本地会话", SUCCESS)
        except Exception as exc:
            self._show_error(str(exc))

    def _check(self, _):
        passed, items = self.lab.check()
        if self.lab.port:
            self.endpoint.value = f'<div class="academy-note">C++ service: <code>{LOOPBACK}:{self.lab.port}</code></div>'
        rows = []
        for item_passed, item in items:
            icon = "✓" if item_passed else "✗"
            rows.append(check_card(f"{icon} {html_module.escape(item)}", SUCCESS if item_passed else FAILURE))
        self.checks.value = "".join(rows)
        self.status.value = check_card("✓ 所有契约通过" if passed else "✗ 当前 C++ 仍不符合契约；这是 Debug 的线索", SUCCESS if passed else FAILURE)

    def show_suite(self):
        self._check(None)

    def close(self):
        self.lab.close()


_panels: dict[str, NetworkPanel] = {}


@atexit.register
def _close_all_network_labs():
    for view in list(_panels.values()):
        with contextlib.suppress(Exception):
            view.close()
    _panels.clear()


def show_network_panel(lab: NetworkLab, profile: str | None = None) -> NetworkPanel:
    old = _panels.pop(lab.name, None)
    if old:
        old.close()
    view = NetworkPanel(lab, profile)
    _panels[lab.name] = view
    get_ipython().display_pub.publish if False else None  # keep type checkers from treating display as a dependency
    from IPython.display import display
    display(view.view)
    return view


def register():
    if getattr(register, "_registered", False):
        return
    @register_cell_magic
    def network_lab(line, cell):
        """%%network_lab <name> <tcp|udp|http|dns|ws|mqtt> [guided|exercise] [pass|fail]"""
        args = line.split()
        if len(args) not in {2, 3, 4}:
            raise ValueError("Usage: %%network_lab <name> <kind> [guided|exercise] [pass|fail]")
        name, kind = args[:2]
        if len(args) >= 3 and args[2] not in {"guided", "exercise"}:
            raise ValueError("profile is guided or exercise")
        if len(args) == 4 and args[3] not in {"pass", "fail"}:
            raise ValueError("expected outcome is pass or fail")
        lab = compile_network_lab(name, kind, cell)
        view = show_network_panel(lab, args[2] if len(args) >= 3 else None)
        get_ipython().user_ns.setdefault("academy_network_panels", {})[name] = view
        if len(args) >= 3:
            passed, _ = lab.check()
            expected = None if len(args) == 3 else args[3] == "pass"
            if expected is not None and passed != expected:
                raise AssertionError("实验的预期结果不一致；检查当前 C++ 与面板线索")

    print("✓ 真实 localhost 网络实验格已准备好。")
    register._registered = True
