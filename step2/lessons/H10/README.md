# H10 — 并发是否真的解决你的问题

入口：[Notebook](../../notebooks/H10-rtos.ipynb)。

传感器、网络和按钮一起工作时开始卡顿。先测瓶颈，再用 Queue 分离工作，避免用更多 Task 制造更多竞态。

前置：H09 主线功能及性能记录。

H02 后使用同一工程。examples/ 为可读的教学示例，不是可一键覆盖学习工程的快照。hints.md 分层，solution.md 与主路径隔离。验证层次见 ../../VALIDATION.md。
