# 分层 Hint

## Hint 1 — 方向
WebSocket handshake 与 frame codec 已在 `run_websocket_server` 中；本课练习只写业务 handler。

## Hint 2 — 关键概念
把 `reminder_on` 放在 lambda 外部，以便不同连接和不同 frame 共享设备状态。

## Hint 3 — 局部思路
先实现 `status`，再实现 `toggle`；都返回同一种 JSON 形状。

## Solution
完成尝试后打开 `notebooks/solutions/17-websocket-live-status-solution.ipynb`。
