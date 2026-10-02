# Lesson 10 — UART 是字节流，不是消息队列

主入口是 [`notebooks/10-uart-terminal.ipynb`](../../notebooks/10-uart-terminal.ipynb)。Notebook 是默认学习路径；本目录保留独立 C++ 变体和行为测试。

## 问题

串口一次读到的内容可能只有半帧，也可能有多帧。把每一次 read 当成一条完整消息会让协议在真实通信中失效。

## 验证

`tests/controller_tests.cpp` 接收 `guided` 或 `exercise`。它只链接当前 `starter`、`challenge` 或 `solution` 的 Controller；不会引用隐藏参考答案。
