# 分层 Hint

## Hint 1 — 方向
连接后先发布 `desk/reminder/state` 的 `offline`，让观察端看见设备上线流程。

## Hint 2 — 关键概念
订阅 `desk/reminder/cmd` 后用 `receive_publish()` 读取 Broker 的真实 PUBLISH packet。

## Hint 3 — 局部思路
topic 是协议契约的一部分；`state` 和 `states` 是完全不同的路由。

## Solution
完成尝试后打开 `notebooks/solutions/18-mqtt-reminder-solution.ipynb`。
