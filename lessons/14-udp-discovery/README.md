# Lesson 14 — 让局域网工具发现提醒器

## 要解决的问题

当还不知道设备 IP 时，一个小工具怎样广播“谁在线”？

## 最小知识

UDP 以 datagram 为单位交付：本地的一次 `sendto` 对应接收方的一次 `recvfrom`。它不保证送达、顺序或重试；本课只验证 localhost 上实际 datagram 的边界。

## 操作与观察

点击“发送 DISCOVER”。看原始 datagram。这里的自动检查只确认本机真实 UDP 往返，不会虚构无线丢包；丢包、重试与设备发现策略将在真实网络环境继续验证。

## 迁移到 STEP 2

迁移到 ESP32 后，socket API 会换为 ESP-IDF/lwIP 适配；发现报文的格式、超时和上层业务逻辑可保留。广播地址、AP 隔离和手机网络权限必须在实网检查。

## 验证

运行 `./tools/run.py` 不能验证网络课；使用本课 `tests/README.md` 中的 `network_verify.py`。
