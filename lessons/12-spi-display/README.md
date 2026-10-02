# Lesson 12 — SPI 事务必须由片选划定边界

主入口是 [`notebooks/12-spi-display.ipynb`](../../notebooks/12-spi-display.ipynb)。Notebook 是默认学习路径；本目录保留独立 C++ 变体和行为测试。

## 问题

SPI 没有地址；片选（CS）决定哪些字节属于哪个设备。若在 CS 未选中时写显示内容，设备不该悄悄接受。

## 验证

`tests/controller_tests.cpp` 接收 `guided` 或 `exercise`。它只链接当前 `starter`、`challenge` 或 `solution` 的 Controller；不会引用隐藏参考答案。
