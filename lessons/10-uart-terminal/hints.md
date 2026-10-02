# Lesson 10 — UART 是字节流，不是消息队列 — 分层 Hint

## Hint 1 — 方向

本课用换行结束帧：`PING\n` 返回 `PONG\n`，`LED:1\n` / `LED:0\n` 返回 `OK\n`。`input.text` 是这次到达的字节，Controller 必须积累到换行才解析。

## Hint 2 — 关键概念

先把输入、持久状态和输出分开。每次 `tick()` 只做一次短判断。

## Hint 3 — 局部思路

先写一个最小状态变量；为每个边界输入写下运行前后应该是什么。不要直接复制 Solution。
