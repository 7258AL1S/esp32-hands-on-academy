"""Compile the C++ edited in a notebook, then show its actual observations."""
from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
import re
import shutil
import subprocess

ROOT = Path(__file__).resolve().parents[1]


@dataclass
class Lab:
    directory: Path

    def executable(self, name: str) -> Path:
        for relative in (name, name + ".exe", "Debug/" + name + ".exe"):
            candidate = self.directory / relative
            if candidate.is_file():
                return candidate
        raise FileNotFoundError(f"Missing executable: {name}")

    def trace(self) -> None:
        from notebook_widgets import show_panel
        return show_panel(self, "trace")

    def test_result(self, profile):
        return subprocess.run([str(self.executable("controller_tests")), profile],
                              capture_output=True, text=True, timeout=10)

    def check(self, profile: str = "exercise", *, expect_pass: bool | None = None, quiet=False) -> bool:
        if profile not in {"guided", "exercise"}:
            raise ValueError("profile must be guided or exercise")
        result = self.test_result(profile)
        if not quiet:
            print(result.stdout, end="")
        if result.returncode not in (0, 1):
            raise RuntimeError(result.stderr or "Test program failed to run")
        passed = result.returncode == 0
        if expect_pass is not None and passed != expect_pass:
            raise AssertionError("Unexpected test outcome; inspect the observations above")
        return passed


def compile_lab(name: str, header: str, source: str) -> Lab:
    """Each rebuild compiles the supplied cell text, never a hidden solution."""
    if not re.fullmatch(r"[a-z][a-z0-9_-]*", name):
        raise ValueError("Use an English lab name such as guided or exercise")
    from notebook_widgets import deactivate
    deactivate(name)
    cmake = shutil.which("cmake")
    if not cmake:
        raise RuntimeError("找不到 CMake。安装 CMake 后重启 VS Code，并重试本格。")
    directory = ROOT / ".build" / "notebook" / name
    source_dir = directory / "source"
    build_dir = directory / "build"
    source_dir.mkdir(parents=True, exist_ok=True)
    (source_dir / "controller.hpp").write_text(header, encoding="utf-8")
    (source_dir / "controller.cpp").write_text(source, encoding="utf-8")
    commands = [
        [cmake, "-S", str(ROOT), "-B", str(build_dir),
         "-DLAB_VARIANT=starter", f"-DLAB_SOURCE_DIR={source_dir}", "-DLAB_PROFILE=exercise"],
        [cmake, "--build", str(build_dir), "--config", "Debug", "--parallel"],
    ]
    for command in commands:
        result = subprocess.run(command, capture_output=True, text=True)
        if result.returncode:
            print(result.stdout)
            print(result.stderr)
            raise RuntimeError("C++ 构建失败；先读上面的 compiler 诊断，修改代码后重跑本格。")
    print(f"✓ 已编译本格 C++：{name}")
    return Lab(build_dir)
