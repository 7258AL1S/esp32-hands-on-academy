# Lesson 07 — 从 ADC 数字读数得到可信电压

主入口是 [`notebooks/07-adc-sensor.ipynb`](../../notebooks/07-adc-sensor.ipynb)。Notebook 是默认学习路径；本目录保留独立 C++ 变体和行为测试。

## 问题

ADC 给你的只是数字。要把旋钮或传感器读数用于决策，必须知道量化范围、参考电压和故障边界。

## 验证

`tests/controller_tests.cpp` 接收 `guided` 或 `exercise`。它只链接当前 `starter`、`challenge` 或 `solution` 的 Controller；不会引用隐藏参考答案。
