# Lesson 17 — 让状态实时推到页面

## 要解决的问题

轮询 HTTP 状态太频繁时，怎样让浏览器与设备保持一条双向连接？

## 最小知识

WebSocket 先用 HTTP Upgrade 建立连接，再交换带 opcode、长度和客户端 mask 的帧。本课的 C++ server 真正完成 101 握手、mask 解码和 text frame；为保持聚焦，只支持短文本帧。

## 操作与观察

先发送 status frame，再发送 toggle frame，最后再发送 status。注意自动检查会创建新连接读取状态，确认设备状态没有只存在于某一个 WebSocket 客户端中。

## 迁移到 STEP 2

ESP32 的 WebSocket server 需要考虑多个客户端、广播策略、最大 frame、心跳和断开清理。协议选择应由实时性和客户端数量决定，不是 HTTP 的“高级版”。

## 验证

运行 `./tools/run.py` 不能验证网络课；使用本课 `tests/README.md` 中的 `network_verify.py`。
