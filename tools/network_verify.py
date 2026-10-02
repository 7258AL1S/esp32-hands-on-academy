#!/usr/bin/env python3
"""Run the network lesson contracts through real localhost I/O.

This checker compiles the source selected from each lesson directory, starts
the resulting C++ program, and drives it with a socket or the local MQTT
fixture. It never imports a student's handler into Python.
"""
from __future__ import annotations

import argparse
from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parent))
from network_lab import compile_network_lab

ROOT = Path(__file__).resolve().parents[1]
LESSONS = {
    13: ("tcp", "tcp-status-service"),
    14: ("udp", "udp-discovery"),
    15: ("http", "http-rest-json"),
    16: ("dns", "connection-state-and-dns"),
    17: ("ws", "websocket-live-status"),
    18: ("mqtt", "mqtt-reminder"),
}


def run_variant(number: int, variant: str, expected: bool) -> bool:
    kind, slug = LESSONS[number]
    source_path = ROOT / "lessons" / f"{number:02d}-{slug}" / variant / "main.cpp"
    lab = compile_network_lab(f"verify-{number}-{variant}", kind, source_path.read_text(encoding="utf-8"))
    try:
        passed, details = lab.check()
    finally:
        lab.close()
    result = passed == expected
    mark = "PASS" if result else "FAIL"
    print(f"[{mark}] Lesson {number:02d} {variant}: actual={'pass' if passed else 'fail'}, expected={'pass' if expected else 'fail'}")
    for item_passed, detail in details:
        print(f"  - {detail}")
    return result


def main() -> int:
    parser = argparse.ArgumentParser(description="验证 STEP 1 网络课的真实 localhost 契约")
    parser.add_argument("--lesson", type=int, choices=sorted(LESSONS), help="只验证一课")
    parser.add_argument("--variant", choices=("starter", "challenge", "solution"), help="只验证一个实现")
    args = parser.parse_args()
    numbers = [args.lesson] if args.lesson else sorted(LESSONS)
    variants = [args.variant] if args.variant else ["starter", "challenge", "solution"]
    all_passed = True
    for number in numbers:
        for variant in variants:
            expected = variant == "solution"
            all_passed = run_variant(number, variant, expected) and all_passed
    if all_passed:
        print("[PASS] 所有网络课都通过了预期契约")
        return 0
    print("[FAIL] 至少一个实现与预期契约不一致")
    return 1


if __name__ == "__main__":
    raise SystemExit(main())
