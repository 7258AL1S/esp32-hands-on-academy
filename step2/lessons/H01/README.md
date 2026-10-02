# H01 — 把虚拟按钮换成真实 GPIO

入口：[Notebook](../../notebooks/H01-gpio.ipynb)。

STEP 1 的“按下”是一个软件输入。现在我们要验证真实按钮的电平、真实 LED 的电流回路，而不把串口状态当作灯真的亮了。

前置：H00 的构建、烧录和启动日志通过；LED、电阻、按钮、面包板。

H02 后使用同一工程。examples/ 为可读的教学示例，不是可一键覆盖学习工程的快照。hints.md 分层，solution.md 与主路径隔离。验证层次见 ../../VALIDATION.md。
