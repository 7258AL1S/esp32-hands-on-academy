# 分层 Hint

## Hint 1 — 方向
先只实现 GET，并将 `bool` 拼成 JSON 的 `true` 或 `false`，不要加引号。

## Hint 2 — 关键概念
POST 的 body 就是当前实验台送来的精确 JSON 字符串。

## Hint 3 — 局部思路
不要在 POST 分支重新声明 `bool reminder_on`；这样会遮蔽外层保存的设备状态。

## Solution
完成尝试后打开 `notebooks/solutions/15-http-rest-json-solution.ipynb`。
