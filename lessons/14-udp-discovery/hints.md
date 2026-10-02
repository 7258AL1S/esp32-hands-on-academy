# 分层 Hint

## Hint 1 — 方向
UDP handler 得到的是完整 datagram，所以这里直接比较 `packet == "DISCOVER"`。

## Hint 2 — 关键概念
验收使用精确字符串；不要添加 `\n`。

## Hint 3 — 局部思路
对未知 datagram 返回短错误值即可，先不要自造重试机制。

## Solution
完成尝试后打开 `notebooks/solutions/14-udp-discovery-solution.ipynb`。
