# Lesson 06 — 用 PWM 调节亮度

主入口是 [`notebooks/06-pwm-dimmer.ipynb`](../../notebooks/06-pwm-dimmer.ipynb)。Notebook 是默认学习路径；本目录保留独立 C++ 变体和行为测试。

## 问题

LED 亮度不是只有开和关。PWM 用快速开关的占空比控制平均能量，让人眼看到不同亮度。

## 验证

`tests/controller_tests.cpp` 接收 `guided` 或 `exercise`。它只链接当前 `starter`、`challenge` 或 `solution` 的 Controller；不会引用隐藏参考答案。
