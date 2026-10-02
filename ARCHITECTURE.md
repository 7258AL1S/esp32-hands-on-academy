# 技术架构

## 为什么 STEP 1 先用原生 C++

第一阶段的目标是让学习者在没有开发板时真正运行代码。原生 C++ + CMake 只依赖普通电脑上的编译器，构建快、测试容易、没有模拟真实芯片的错觉。Virtual Board 只模拟课程需要的接口；它不会声称模拟 GPIO 电气、定时器精度、Interrupt 延迟或网络栈。

课程使用 VS Code `.ipynb`，文字和 C++ 实验交替排列。标准 Python kernel 只负责运行支架：初始化格注册 `%%cpp_lab`，后续整格内容写入 C++ 源文件，经 CMake 构建后运行。学习者不需要 Python 基础；没有把控制逻辑重新写成 Python。

选择这个支架是为了复用 VS Code 成熟的 Jupyter 扩展，避免第一课先安装和排查额外的 C++ kernel。代价是 Notebook 的自动补全以 Python kernel 为主，C++ 编辑体验不如独立 `.cpp`；compiler 诊断依然是真实的 C++ 诊断。需要完整 C++ IDE 功能时，可以在生成的源码或独立固件工程中调试。

## STEP 1 的边界

```text
Notebook 内本格 Controller (C++)
        ↓ 依赖最小 Board contract
VirtualBoard / test double
        ↓ 通过 CMake + C++ 行为测试验证本格代码
电脑
```

`Board` 目前只有 `read_button()` 和 `write_led()`。接口很小是有意的：学习者可以看懂依赖，测试可以控制输入并检查输出。

## STEP 2 的迁移

```text
同一个 Controller
        ↓
Esp32Board : Board
        ├── pinMode(button_pin, INPUT_PULLUP)
        ├── digitalRead(button_pin)
        └── digitalWrite(led_pin, on ? HIGH : LOW)
```

Arduino 适配器会是第一种硬件迁移；当需求需要更细的任务、驱动或组件依赖时，再增加 ESP-IDF 适配器。上层 `Controller` 不直接调用 `digitalRead`、`gpio_get_level` 等 API。具体 GPIO 编号、电平极性和板卡接线应由硬件实现和接线表决定。

## 自动验证和证据边界

STEP 1 测试检查状态转换和边界行为。STEP 2 另外提供 Hardware Checklist：LED 是否真实亮灭、按钮是否可靠、串口输出是否符合预期。电脑测试不能验证真实电平、抖动、供电、芯片复位、Wi-Fi 射频或 OTA。

## 有意不做的事

当前切片使用 Notebook 作为默认学习界面；不引入 PlatformIO、FreeRTOS 或网络库。它们会在对应问题出现时加入。

## Notebook 执行与答案隔离

每个 `%%cpp_lab` 格编译一个完整 Controller，并启动一个持久的 host C++ 进程。面板通过行协议发送 press/release/tick/reset，读取真实程序输出；Python 控件不替学生控制 LED。代码格之间不共享 Controller。生成源码和构建位于 `.build/notebook/<实验名>/`；测试和模拟器都链接本格实现。

`ipywidgets` 提供按钮、单步、慢放、复位和动画检查。每个 tick 推进 10 ms 虚拟时间，慢放每 250 ms 实际时间推进一个 tick。首次偏离契约保留红色提示，直到复位。重编译前停用同名旧面板并关闭进程，kernel 退出时清理进程，读取超时会终止无响应的 C++。

后续外设逐课扩展同一管线：输入控件 → 小型 HAL 合约 → C++ → 可视化观测 → 行为契约。现在只有按钮与 LED；传感器、波形和总线视图在课程计划中。板卡示意和电流计算不是芯片/电路仿真器。

## 可视化 UI 的共同约束

`tools/notebook_theme.py` 集中管理深色主题；针脚探索和实验台复用同一面板、控件行与检查卡片。面板使用 VS Code 风格的 `#1e1e1e` 背景，图示与检查卡片使用 `#141414`，正文为近白色。CSS 只作用于 Academy 面板及承载它的 VS Code widget 输出容器，避免影响其他课程内容。

后续传感器或总线 UI 必须遵循以下规则：

- 长说明放在图外的 HTML 文本区，允许自然换行；不把段落压进板卡形状或 SVG 页脚。
- SVG 标签留在各自区域内；较长的引脚用途分行，保留左右边距。图示宽度随面板缩放。
- 控件行允许折行；Dropdown、Slider 和文字区宽度不超过父面板。避免固定高度裁切说明。
- 成功、失败、按下和 LED 状态同时提供文字或符号，不能只靠颜色传达。新增颜色需检查文字与实际背景的对比度，普通文字至少 4.5:1。
- 验证必须包含 VS Code 实际渲染：检查普通宽度与较窄面板、按钮两种状态、失败提示、自动检查动画、Dropdown 和 Slider。只通过 C++ 测试不足以证明排版正确。

练习格仅编译学生文本，绝不自动引用 `solution/`。主 Notebook 中故障和未完成练习的失败是正常反馈；参考答案在独立 Notebook。维护者通过真实 Jupyter kernel 从头执行两份 Notebook，默认将输出留在内存中，交付文件不携带过期 widget 状态；只有显式 `--save` 才写回示范输出。
