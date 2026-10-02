# Lesson 18 — 让提醒器接入消息总线

## 要解决的问题

多个设备和工具都要收发提醒器事件时，为什么不让每个设备都直接连彼此？

## 最小知识

MQTT Client 连接 Broker，向 topic 发布或订阅。QoS 0 表示尽力而为；它不保证送达。本课由 Python MiniBroker 提供一个真实 localhost MQTT 3.1.1 QoS 0 对端，学生 C++ Client 负责 CONNECT、SUBSCRIBE、接收命令与 PUBLISH。

## 操作与观察

点击“运行 MQTT Client”。Broker 在订阅成功后向 `desk/reminder/cmd` 发出 `toggle`；你的 C++ 先发布 offline，再根据命令发布 on。面板展示 Broker 实际收到的 topic 和 payload。

## 迁移到 STEP 2

迁移到 ESP32 时使用 ESP-MQTT 或等效 adapter，保留 topic 和业务状态机。真实项目还需要 Broker 地址、证书/认证、重连、offline 遗嘱和 QoS 选择。MiniBroker 不验证这些生产条件。

## 验证

运行 `./tools/run.py` 不能验证网络课；使用本课 `tests/README.md` 中的 `network_verify.py`。
