# Lesson 08 — 把传感器细节留在 Driver 层

主入口是 [`notebooks/08-driver-hal.ipynb`](../../notebooks/08-driver-hal.ipynb)。Notebook 是默认学习路径；本目录保留独立 C++ 变体和行为测试。

## 问题

上层提醒器只需要一个可靠温度值，却不应到处处理连接、故障和校准细节。

## 验证

`tests/controller_tests.cpp` 接收 `guided` 或 `exercise`。它只链接当前 `starter`、`challenge` 或 `solution` 的 Controller；不会引用隐藏参考答案。
