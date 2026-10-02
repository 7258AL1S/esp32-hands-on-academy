# Lesson 11 — I2C 地址、ACK 和设备缺失

主入口是 [`notebooks/11-i2c-sensor.ipynb`](../../notebooks/11-i2c-sensor.ipynb)。Notebook 是默认学习路径；本目录保留独立 C++ 变体和行为测试。

## 问题

I2C 总线共享两根线，主机必须确认目标地址和应答。读不到设备时继续把旧值当新数据会误导上层。

## 验证

`tests/controller_tests.cpp` 接收 `guided` 或 `exercise`。它只链接当前 `starter`、`challenge` 或 `solution` 的 Controller；不会引用隐藏参考答案。
