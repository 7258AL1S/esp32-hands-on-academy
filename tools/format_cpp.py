"""Format Lesson 02+ C++ sources and Notebook cells without touching Python or notes."""
from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
MAGICS = ("%%academy_lab ", "%%network_lab ", "%%cpp_lab ")
# Keep literals intact; comment whitespace and ordinary whitespace have no
# effect on the program. Reject any formatter change to the other tokens.
TOKEN = re.compile(
    r'(?P<raw>(?:u8|u|U|L)?R"(?P<delimiter>[^ ()\\\t\r\n]{0,16})'
    r'\(.*?\)(?P=delimiter)")'
    r'|(?P<comment>//[^\n]*|/\*.*?\*/)'
    r'|(?P<literal>(?:u8|u|U|L)?"(?:\\.|[^"\\])*"'
    r"|(?:u8|u|U|L)?'(?:\\.|[^'\\])*')"
    r'|(?P<word>[A-Za-z_][A-Za-z_0-9]*|[0-9]+(?:\.[0-9]+)?)'
    r'|(?P<operator>::|->\*|->|\+\+|--|<<=|>>=|<<|>>|&&|\|\|'
    r'|==|!=|<=|>=|\+=|-=|\*=|/=|%=|&=|\|=|\^=|\.\*|\.\.\.)'
    r'|(?P<other>\S)',
    re.DOTALL,
)


def tokens(source: str) -> list[str]:
    return [re.sub(r"\s+", " ", match.group()) if match.lastgroup == "comment"
            else match.group() for match in TOKEN.finditer(source)]


def format_source(compiler: str, source: str) -> str:
    result = subprocess.run(
        [compiler, "--style=file", f"--assume-filename={ROOT / 'controller.hpp'}"],
        input=source, capture_output=True, text=True, check=True,
    ).stdout
    if tokens(source) != tokens(result):
        raise ValueError("Formatting changed C++ tokens; no files have been written")
    return result


def main() -> None:
    parser = argparse.ArgumentParser(description="整理第二课起的 C++ 与 Notebook 代码格")
    parser.add_argument("--check", action="store_true", help="只检查，不写入文件")
    parser.add_argument("--clang-format", help="clang-format 的路径")
    args = parser.parse_args()
    bundled = Path(sys.executable).parent / ("clang-format.exe" if os.name == "nt"
                                            else "clang-format")
    local = ROOT / ".venv" / ("Scripts/clang-format.exe" if os.name == "nt"
                              else "bin/clang-format")
    compiler = args.clang_format or shutil.which("clang-format")
    if not compiler and local.is_file():
        compiler = str(local)
    if not compiler and bundled.is_file():
        compiler = str(bundled)
    if not compiler:
        parser.error("请安装 clang-format，或用 --clang-format 指定路径")

    pending: dict[Path, str] = {}
    cells = 0
    sources = 0
    for lesson in sorted((ROOT / "lessons").glob("[0-9][0-9]-*")):
        if lesson.name.startswith("01-"):
            continue
        for path in sorted(lesson.rglob("*")):
            if path.suffix not in {".cpp", ".hpp"}:
                continue
            before = path.read_text(encoding="utf-8")
            after = format_source(compiler, before)
            sources += 1
            if before != after:
                pending[path] = after

    for path in sorted((ROOT / "notebooks").rglob("*.ipynb")):
        if path.name.startswith("01-"):
            continue
        notebook = json.loads(path.read_text(encoding="utf-8"))
        changed = False
        for cell in notebook["cells"]:
            if cell["cell_type"] != "code":
                continue
            source = cell["source"]
            before = "".join(source) if isinstance(source, list) else source
            if not before.startswith(MAGICS):
                continue
            magic, code = before.split("\n", 1)
            after = magic + "\n" + format_source(compiler, code)
            cells += 1
            if before != after:
                cell["source"] = after.splitlines(keepends=True)
                changed = True
        if changed:
            pending[path] = json.dumps(notebook, ensure_ascii=False, indent=1) + "\n"

    for path in pending:
        print(path.relative_to(ROOT))
    print(f"检查 {cells} 个 C++ 格、{sources} 个源文件；"
          f"{'需整理' if args.check else '已整理'} {len(pending)} 个文件。")
    if args.check:
        raise SystemExit(bool(pending))
    for path, content in pending.items():
        path.write_text(content, encoding="utf-8")


if __name__ == "__main__":
    main()
