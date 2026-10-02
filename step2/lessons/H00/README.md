# H00 — 把第一份固件送进 ESP32

入口：[Notebook](../../notebooks/H00-setup.ipynb)。

电脑能编译 C++，为什么还不能直接把这个程序放进 ESP32？今天只完成一条链：工具链 → 固件 → Flash → 启动日志。

前置：STEP 1 可选完成；准备板和数据线。

H02 后使用同一工程。examples/ 为可读的教学示例，不是可一键覆盖学习工程的快照。hints.md 分层，solution.md 与主路径隔离。验证层次见 ../../VALIDATION.md。
