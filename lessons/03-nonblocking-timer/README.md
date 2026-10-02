# Lesson 03 — 计时，但不要把程序卡住

主入口是 [`notebooks/03-nonblocking-timer.ipynb`](../../notebooks/03-nonblocking-timer.ipynb)。Notebook 是默认学习路径；本目录保留独立 C++ 变体和行为测试。

## 问题

桌面提醒器开始倒计时后，仍必须响应取消按钮。把 `delay(3000)` 放进 `tick()` 会让程序在等待期间失去响应。

## 验证

`tests/controller_tests.cpp` 接收 `guided` 或 `exercise`。它只链接当前 `starter`、`challenge` 或 `solution` 的 Controller；不会引用隐藏参考答案。
