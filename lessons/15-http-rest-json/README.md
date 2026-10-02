# Lesson 15 — 给提醒器做一个 HTTP 控制接口

## 要解决的问题

电脑怎样用浏览器友好的方式读取并修改提醒器？

## 最小知识

HTTP 把 method、path、headers 和 body 组织成请求；REST 只是围绕资源设计接口的约定。JSON 是数据格式，不等于网络协议。这个微型 server 只实现本课所需的最小子集。

## 操作与观察

先 GET 状态，再 POST 开启/关闭，然后再 GET。面板显示真实 HTTP status、content type 和 JSON body；C++ 决定响应内容与状态变化。

## 迁移到 STEP 2

上层的 URL、JSON 与状态机可迁移到 ESP32 的 HTTP server adapter。真实设备要限制请求体、避免阻塞传感器任务，并添加认证或至少局域网边界说明。

## 验证

运行 `./tools/run.py` 不能验证网络课；使用本课 `tests/README.md` 中的 `network_verify.py`。
