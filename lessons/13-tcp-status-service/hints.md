# 分层 Hint

## Hint 1 — 方向
先把状态放到 `run_tcp_line_server` 外侧、`main()` 生命周期内。

## Hint 2 — 关键概念
请求中包含换行；比较时也要包含 `\n`。

## Hint 3 — 局部思路
先处理 STATUS，再处理 TOGGLE；未知输入返回可读的错误行。

## Solution
完成尝试后打开 `notebooks/solutions/13-tcp-status-service-solution.ipynb`。
