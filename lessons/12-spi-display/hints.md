# Lesson 12 — SPI 事务必须由片选划定边界 — 分层 Hint

## Hint 1 — 方向

本课文本事务是 `CS:LOW\n`、`WRITE:文本\n`、`CS:HIGH\n`。只有在 LOW 到 HIGH 的完整事务结束时，虚拟显示器更新 `display`。

## Hint 2 — 关键概念

先把输入、持久状态和输出分开。每次 `tick()` 只做一次短判断。

## Hint 3 — 局部思路

先写一个最小状态变量；为每个边界输入写下运行前后应该是什么。不要直接复制 Solution。
