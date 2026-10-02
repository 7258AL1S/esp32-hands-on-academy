#!/usr/bin/env python3
"""Build/run one lesson variant without requiring a Python package."""
from __future__ import annotations

import argparse
import os
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BUILD_ROOT = ROOT / ".build"


def run(command: list[str]) -> None:
    print("$ " + " ".join(command))
    subprocess.run(command, cwd=ROOT, check=True)


def main() -> int:
    parser = argparse.ArgumentParser(description="Run ESP32 Academy Lesson 01")
    parser.add_argument("mode", choices=["guided", "challenge", "exercise", "simulate", "interactive"])
    args = parser.parse_args()
    variant = {"guided": "starter", "challenge": "challenge", "exercise": "solution", "simulate": "solution", "interactive": "solution"}[args.mode]
    # Separate trees keep independent terminals or CI jobs from racing while
    # CMake is replacing the controller library for another variant.
    BUILD = BUILD_ROOT / args.mode
    profile = "guided" if variant == "starter" else "exercise"
    cmake = shutil.which("cmake")
    if cmake is None:
        parser.error("CMake 3.16+ is required")
    configure = [cmake, "-S", str(ROOT), "-B", str(BUILD), f"-DLAB_VARIANT={variant}", f"-DLAB_PROFILE={profile}"]
    run(configure)
    run([cmake, "--build", str(BUILD), "--parallel"])
    if args.mode in {"guided", "challenge", "exercise"}:
        result = subprocess.run([cmake, "--build", str(BUILD), "--target", "controller_tests"], cwd=ROOT)
        if result.returncode:
            return result.returncode
        return subprocess.run([str(BUILD / "controller_tests"), profile], cwd=ROOT).returncode
    simulator = str(BUILD / "simulator")
    return subprocess.run([simulator] + (["--interactive"] if args.mode == "interactive" else []), cwd=ROOT).returncode


if __name__ == "__main__":
    sys.exit(main())
