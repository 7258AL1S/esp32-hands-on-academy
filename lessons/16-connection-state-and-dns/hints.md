# 分层 Hint

## Hint 1 — 方向
先判断 `academy::net::env_flag("ACADEMY_WIFI_CONNECTED")`，离线时立即 return。

## Hint 2 — 关键概念
DNS 与连接分别调用 `resolve_ipv4("localhost")`、`request_line("localhost", port, "PING\n")`。

## Hint 3 — 局部思路
本课的 Wi-Fi bool 只是模型；输出中真正可验证的是 DNS 解析和 TCP PONG。

## Solution
完成尝试后打开 `notebooks/solutions/16-connection-state-and-dns-solution.ipynb`。
