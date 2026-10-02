# Lesson 05 — 用固定资源处理连续数据

主入口是 [`notebooks/05-embedded-cpp.ipynb`](../../notebooks/05-embedded-cpp.ipynb)。Notebook 是默认学习路径；本目录保留独立 C++ 变体和行为测试。

## 问题

嵌入式设备不能假设内存无限。一个不断接收采样数据的工具需要明确容量、错误和数据所有权。

## 验证

`tests/controller_tests.cpp` 接收 `guided` 或 `exercise`。它只链接当前 `starter`、`challenge` 或 `solution` 的 Controller；不会引用隐藏参考答案。
