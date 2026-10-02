# Lesson 09 — 短脉冲来了，Polling 会漏掉吗？

主入口是 [`notebooks/09-interrupts.ipynb`](../../notebooks/09-interrupts.ipynb)。Notebook 是默认学习路径；本目录保留独立 C++ 变体和行为测试。

## 问题

一个短脉冲可能在下一次 Polling 前已经消失。中断（Interrupt）让硬件先把事件计数，主循环随后消费计数。

## 验证

`tests/controller_tests.cpp` 接收 `guided` 或 `exercise`。它只链接当前 `starter`、`challenge` 或 `solution` 的 Controller；不会引用隐藏参考答案。
