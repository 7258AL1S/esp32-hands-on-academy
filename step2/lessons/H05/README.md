# H05 — 给工具接上外部设备

入口：[Notebook](../../notebooks/H05-buses.ipynb)。

外设不是“调用一次 API 就有数据”。本单元拆成 UART、I2C、SPI 三个小实验，一次只接一个；把超时、半帧和事务语义留在驱动层。

前置：H04；UART/SPI 只需跳线，BH1750 为 I2C 实验所需。

H02 后使用同一工程。examples/ 为可读的教学示例，不是可一键覆盖学习工程的快照。hints.md 分层，solution.md 与主路径隔离。验证层次见 ../../VALIDATION.md。
