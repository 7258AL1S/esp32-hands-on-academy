# Lesson 04 — 给计时器一张状态图 — 分层 Hint

## Hint 1 — 方向

有限状态机（state machine）让每个时刻只有一个状态。命令从 `input.text` 进入：`start`、`pause`、`resume`、`reset`。同一条命令不会因多次 tick 重复执行。

## Hint 2 — 关键概念

先把输入、持久状态和输出分开。每次 `tick()` 只做一次短判断。

## Hint 3 — 局部思路

先写一个最小状态变量；为每个边界输入写下运行前后应该是什么。不要直接复制 Solution。
