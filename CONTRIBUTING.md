# Contributing

欢迎提交中文教学体验、可复现的 Bug 报告和小型改进。

提交 Lesson 变更时，请说明：学习者遇到的问题、最小新增知识、运行命令、测试结果，以及哪些行为仍需要真实硬件验证。请保持代码、API、协议和文件名使用英文；面向学习者的解释使用简体中文。

新增 Lesson 前先完成一个垂直切片，并在课程维护者确认现有切片的学习体验稳定后再扩展。不要在没有行为测试或硬件清单的情况下添加“看起来能跑”的示例。

## C++ 教学代码的可读性

Notebook 中的代码格与独立 `.cpp` / `.hpp` 都是学习材料，不能压成一行。统一使用根目录 `.clang-format`：4 空格缩进、88 列换行，类、函数、分支和循环展开；保留原有注释、字符串和代码逻辑。

维护者可安装 `clang-format`（当前使用 23.1.2）并整理第二课起的代码：

```sh
.venv/bin/python -m pip install clang-format==23.1.2
.venv/bin/python tools/format_cpp.py
.venv/bin/python tools/format_cpp.py --check
```

Windows PowerShell 可将 `.venv/bin/python` 替换为 `.venv\Scripts\python`。这个工具只整理 C++ 代码格和 Lesson 02 起的 C++ 源文件，不改正文、Python 支架或保存的学习记录；第一课作为已使用的 UI 基准保持原样。它保留 magic 首行，并在写入前检查 C++ token 未变。学习者无需安装格式工具。

## STEP 2 贡献

遵循 step2/TEACHING.md，主 Notebook 的当前步骤/模式/示例与 step2/course.json 同步。
Guided 示例修改后同步预览格，运行 tools/verify_step2.py；显示变更运行 --notebooks，
API/固件变更在 v5.5.1 环境运行 --firmware，涉及硬件另外记录实物证据。
保留用户工程、不预读 Solution，不把人工检查或模拟通过称为实机验证。
