# Lesson 22：配置落盘与版本迁移

`%config_store` 打开真实的本地沙箱。写入采用临时文件后原子替换，v1 的 `name`/`interval_s` 会迁移为 v2 的 `device_name`/`report_interval_ms`。这是真实文件操作，不是 UI 状态动画；在 ESP32 上相同思路对应 NVS 事务和掉电验证。

请先写入配置，再关闭并重新执行相关代码格观察它仍然存在；再写入 v1 文件并跑测试确认迁移。
