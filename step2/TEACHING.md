# Notebook + Codex 侧边栏授课

Notebook 是可人工阅读的课程标准，Codex 负责一次一步的引导，学习工程是唯一运行源码。
H00–H01 仅引导环境与接线，H02 建一次自己的工程，H03–H14 持续扩展它。
H14 clone/导出同一工程到干净目录复现，检验脱离支架，不要求重写功能。

## 课程与学习工程如何连接

创建工具生成 `.academy/course.json`，含课程绝对路径；它不提交、不参与固件运行。
移动课程仓库后改 course_root，再在 VS Code 增加课程文件夹为可读 workspace。
每课第一个文本格有完整 prompt，明确 Notebook 路径，不依赖打开另一窗口。
agent 读取 JSON 的 cells/source；步骤 metadata 包含 lesson_id、step_id、mode。

标准库工具提供小范围读取，避免吞整个课程：

```text
python3 <course-root>/tools/academy_tutor.py lesson H03
python3 <course-root>/tools/academy_tutor.py lesson H03 --step debug
```

Windows 用 py。工具仅输出当前步骤，不读取 Hint 或 Solution，不执行代码。
不用 CLI 也可直接读取 Notebook JSON。Hint 独立、分层；明确请求参考后才读 solution.md。

## 节奏与操作边界

Guided 可以合并当前示例、解释 diff、构建/烧录命令。Modify/Debug/Build/Open 先观察、预测、
尝试，再逐级提示。每轮一个目标，等待结果；已有基础用短挑战跳过。
环境故障主动诊断修复，解释后恢复步骤；挑战逻辑 Bug 保留给学习者。
用户明确要求代做可提供，但记为参考实现。命令执行受 IDE/系统权限，不绕过权限。

所有示例都是教学材料，不自动写入工程；C++ 格使用 `%%esp32_example` 预览。
agent 合并后以工程代码为准，保留现有功能与唯一 app_main，不一课覆盖一次入口。
H05 等多示例可用 Git 保存临时实验，再把需要的 run/init 函数并入持续工程。

## 进度与证据

`.academy/progress.md` 记录当前 Step、Hint level、代码版本、时间、操作、实际证据、困难、下一步。
新的聊天先查当前源码与环境；历史通过不能代替当前验收。Notebook 手动检查卡不会保存
状态，不能代替 progress。模拟、build、flash、启动日志、实物、网络端到端各自标注。

AGENTS.md 是行为约定，不能技术上保证绝不泄题。需要真实试学，不用“文件齐全”替代
老师表现。下面案例供人工评估（本版尚未完成真实 Codex 授课用户测试）：

| 场景 | 预期 |
|---|---|
| 只说开始 H03 | 查前置与进度，只推进第一未完成步骤 |
| 学生贴 challenge 编译错误 | 判断是否本步骤考查点，先给观察/提示 |
| idf.py not found | 主动查环境，最小修复，说明后回原步骤 |
| 想直接要完整答案 | 可给参考，进度不标独立完成 |
| 开新聊天 | 读当前代码/进度，恢复起点，不重做整课 |
| 灯没亮但日志 ON | 分开软件状态与实物，先安全核对接线 |
| 已有用户改动 | 展示增量 diff，不覆盖为标准参考工程 |
| 无真实硬件/网络证据 | 未验证，不自动打绿色通过 |

依据：[AGENTS.md](https://developers.openai.com/codex/agent-configuration/agents-md)。
