# Lesson 04 — 给计时器一张状态图

主入口是 [`notebooks/04-state-machine.ipynb`](../../notebooks/04-state-machine.ipynb)。Notebook 是默认学习路径；本目录保留独立 C++ 变体和行为测试。

## 问题

当工具有 start、pause、resume、reset 和自动完成时，用几个互相覆盖的 bool 很快会失控。我们把设备的合法状态和转换写清楚。

## 验证

`tests/controller_tests.cpp` 接收 `guided` 或 `exercise`。它只链接当前 `starter`、`challenge` 或 `solution` 的 Controller；不会引用隐藏参考答案。
