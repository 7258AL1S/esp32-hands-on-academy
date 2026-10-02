# Lesson 16 — 连接前先判断网络状态

## 要解决的问题

设备在网络尚未可用时，为什么不应该直接无限重连服务器？

## 最小知识

Wi-Fi 关联、DHCP、DNS 和 TCP 是不同阶段。没有 ESP32 射频时，本课用一个明确标记的 Wi-Fi 状态模型控制是否允许连接；一旦允许，`localhost` 的 DNS 解析和 TCP PING/PONG 都由 C++ 真实执行。

## 操作与观察

分别点击断开与已连接。断开状态应只输出 WAIT_FOR_WIFI；连接状态则由当前 C++ 解析 `localhost` 为 `127.0.0.1` 并连接真实本机 fixture。

## 迁移到 STEP 2

ESP32 上应把 Wi-Fi event、IP obtained event、DNS 失败和指数退避重连组织成状态机。不要将 host 的 `ACADEMY_WIFI_CONNECTED` 当作真实无线验证。

## 验证

运行 `./tools/run.py` 不能验证网络课；使用本课 `tests/README.md` 中的 `network_verify.py`。
