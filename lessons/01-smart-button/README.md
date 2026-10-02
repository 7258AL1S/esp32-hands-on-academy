# Lesson 01 — 每次按下只切换一次

首选入口是 [`notebooks/01-smart-button.ipynb`](../../notebooks/01-smart-button.ipynb)。在 VS Code 中打开它并按顺序运行 cell；本页保留课程地图，方便不使用 Notebook 的读者查阅。

## 1. 我们要解决什么问题？

我们想做一个桌面提醒按钮：按一下，LED 改变状态；松开后状态保持。现在没有 ESP32，所以先用 Virtual GPIO 在电脑上观察它。

## 2. 最小知识

按钮输入有两种常见的观察方式：当前电平（level）和变化事件（edge）。本课的输入约定是 `LOW = pressed`，模拟了常见的 `INPUT_PULLUP` 接法。`tick()` 每次读取一次输入并更新输出。

## 3. 第一个实验（Level 1 — Guided）

运行：

```sh
python3 tools/run.py guided
```

starter 的行为是：没按下 LED 灭，按下 LED 亮，保持按下 LED 仍亮，松开 LED 灭。观察测试名称和模拟输出。思考：这实现的是“输出跟随输入”，还是“按一下切换状态”？

## 4. 修改（Level 2 — Modify）

打开 `starter/controller.cpp`，把输出改成你认为更符合“提醒按钮”的行为。先不要看 `challenge/`。运行 `python3 tools/run.py guided`，然后用 `interactive` 观察。

## 5. 故意制造 Bug（Level 3 — Debug）

```sh
python3 tools/run.py challenge
```

这个版本每次看到 `LOW` 都切换一次。由于程序会反复采样，长按会不停切换。先读失败信息，回答：测试是在检查“按钮现在是 LOW”，还是检查“按钮刚刚从 HIGH 变 LOW”？

## 6. 分层 Hint

- **Hint 1**：需要记住上一次采样到的 pressed 状态。
- **Hint 2**：只在 `pressed == true` 且 `was_pressed == false` 时处理事件。
- **Hint 3**：每次 tick 结束前更新 `was_pressed`；第一次采样需要一个明确的上电策略。

## 7. 实践挑战（Level 4 — Build）

从 `challenge/` 复制一份控制器，自己修复它。要求：释放不改变 LED；一次长按只改变一次；上电时如果按钮已经按住，不把它误算作一次新按下。

## 8. 开放挑战（Level 5 — Open）

为控制器增加一个可测试的 debounce 策略。先写出你要保证的行为，再决定需要时间戳、采样窗口还是外部事件。不要直接假设 VirtualBoard 已经模拟了真实抖动。

## 9. 你刚刚真正学到了什么？

你区分了输入的 level 和 edge，把硬件依赖限制在 `Board` 合约内，并用行为测试描述需求。`Controller` 没有假设 LED 是哪一个 GPIO，也没有把模拟器 API 带进逻辑层。

## 10. 硬件迁移预告

有板子后，先实现 `Esp32Board`：配置 button 为 `INPUT_PULLUP`，配置 LED 为 `OUTPUT`，把读取和写入映射到 `Board`。保留 `Controller` 和 STEP 1 的测试；增加硬件清单：LED 极性、按钮接线、电平、串口日志和复位后的初始状态。

## 验证说明

`tests/controller_tests.cpp` 的 `guided` 检查对应 starter，`exercise` 检查最终按键约定。测试通过不代表真实按键已经 debounce，也不代表任何具体开发板的 GPIO 编号适用。
