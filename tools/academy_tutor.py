"""Read one hardware teaching step or create an empty learner project.

Standard library only. This tool never executes firmware or opens solutions.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path
import shutil

ROOT = Path(__file__).resolve().parents[1]


def load_course(root=ROOT):
    return json.loads((root / "step2/course.json").read_text(encoding="utf-8"))


def init_project(destination: Path, *, guided=False):
    destination = destination.expanduser().resolve()
    if destination.exists() and (not destination.is_dir() or any(destination.iterdir())):
        raise ValueError("目标必须不存在或为空目录；不会覆盖已有工程")
    template = ROOT / "step2" / ("guided-device" if guided else "device-template")
    # Fail before copying if the source is missing.
    if not (template / "main/main.cpp").is_file():
        raise ValueError("课程工程模板缺失")
    shutil.copytree(template, destination, dirs_exist_ok=True,
                    ignore=shutil.ignore_patterns("build", "sdkconfig", "sdkconfig.old"))
    academy = destination / ".academy"
    academy.mkdir(exist_ok=True)
    (academy / "course.json").write_text(json.dumps({
        "course_root": str(ROOT), "manifest": "step2/course.json",
        "start_lesson": "H00" if guided else "H02",
    }, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    if guided:
        progress = academy / "progress.md"
        progress.write_text(progress.read_text(encoding="utf-8").replace("H02", "H00"),
                            encoding="utf-8")
    return destination


def step_content(lesson_id, step_id, root=ROOT):
    course = load_course(root)
    lesson = next((item for item in course["lessons"] if item["id"] == lesson_id), None)
    if lesson is None:
        raise ValueError(f"未知 Lesson: {lesson_id}")
    notebook = json.loads((root / lesson["notebook"]).read_text(encoding="utf-8"))
    selected = [cell for cell in notebook["cells"]
                if cell.get("metadata", {}).get("academy", {}).get("step_id") == step_id]
    if not selected:
        raise ValueError(f"未知步骤: {lesson_id}/{step_id}")
    return "\n\n".join("".join(cell["source"]) for cell in selected)


def main():
    parser = argparse.ArgumentParser(description="读取 STEP 2 的当前步骤，或创建学习工程")
    commands = parser.add_subparsers(dest="command", required=True)
    create = commands.add_parser("init-project")
    create.add_argument("destination", type=Path)
    create.add_argument("--guided", action="store_true", help="H00–H01 引导工程")
    lesson = commands.add_parser("lesson")
    lesson.add_argument("lesson_id")
    lesson.add_argument("--step", help="不指定时只显示步骤目录")
    args = parser.parse_args()
    try:
        if args.command == "init-project":
            print(f"工程已创建: {init_project(args.destination, guided=args.guided)}")
            print("在 VS Code 打开该目录；从本课 Notebook 复制启动 prompt 到 Codex 侧边栏。")
        elif args.step:
            print(step_content(args.lesson_id, args.step))
        else:
            item = next((x for x in load_course()["lessons"]
                         if x["id"] == args.lesson_id), None)
            if item is None:
                raise ValueError(f"未知 Lesson: {args.lesson_id}")
            print(f"{item['id']} — {item['title']}\n{item['notebook']}")
            for step in item["steps"]:
                print(f"{step['id']:12} {step['mode']:8} {step['title']}")
    except (ValueError, OSError) as exc:
        parser.exit(1, f"错误: {exc}\n")


if __name__ == "__main__":
    main()
