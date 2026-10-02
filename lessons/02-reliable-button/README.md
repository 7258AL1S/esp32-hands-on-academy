# Lesson 02 — 让一次按下只算一次

主入口是 [`notebooks/02-reliable-button.ipynb`](../../notebooks/02-reliable-button.ipynb)。Notebook 是默认学习路径；本目录保留独立 C++ 变体和行为测试。

## 问题

机械按钮按下和松开时会短暂抖动。程序如果把每一次电平变化都当成新事件，提醒器会误触发。

## 验证

`tests/controller_tests.cpp` 接收 `guided` 或 `exercise`。它只链接当前 `starter`、`challenge` 或 `solution` 的 Controller；不会引用隐藏参考答案。
