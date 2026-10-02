# Lesson 23：BLE 服务模型与 provisioning

本课用 `connected` 和 `text` 表示 BLE 连接事件与特征写入：设备离线时 advertising，连接后公开 Provisioning Service 和 WiFiCredentials Characteristic，合法写入得到确认，空凭据被拒绝。

这是协议和状态模型，不声称模拟射频、发现、配对、加密或手机系统行为。STEP 2 要用真实 BLE 工具验证这些边界。
