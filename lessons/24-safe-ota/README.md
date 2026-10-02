# Lesson 24：OTA 的校验、候选槽与回滚

`%ota_sandbox` 会真实创建 `firmware.bin` 和 `manifest.txt`，由 C++ `Sha256` 计算并核对包内容，成功后写入非活动候选槽；健康检查失败会删除候选并回滚，成功才确认新槽。

完整 OTA 还需要签名、secure boot、flash encryption、分区表、bootloader 和掉电测试。本课 host 沙箱只证明字节完整性、原子激活和失败恢复，不冒充真实 ESP32 Flash OTA。
