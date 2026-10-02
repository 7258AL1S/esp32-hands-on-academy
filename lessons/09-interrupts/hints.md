# Lesson 09 — 短脉冲来了，Polling 会漏掉吗？ — 分层 Hint

## Hint 1 — 方向

本机模型把 `input.pulses` 视为 ISR 已经累积的单调脉冲数。Controller 不假装在 ISR 中做复杂工作，只计算增量并更新事件数。

## Hint 2 — 关键概念

先把输入、持久状态和输出分开。每次 `tick()` 只做一次短判断。

## Hint 3 — 局部思路

先写一个最小状态变量；为每个边界输入写下运行前后应该是什么。不要直接复制 Solution。
