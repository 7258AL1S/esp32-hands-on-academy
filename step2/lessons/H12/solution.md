# H12 — 尝试后的参考

默认授课路径不读取本文件。明确请求完整参考时再使用；参考实现不记录为独立完成。

“没有广播”的参考判断：已 provisioned 设备本来走普通 Wi-Fi 路径；未 provisioned 才启动 BLE。错误 PoP 的处理是重新核对合法设备的 PoP，不改 SECURITY_1 为 SECURITY_0。重新配网要通过明确操作调用 reset_provisioning，保留失败恢复与本地使用路径。

Guided 示例位于 examples/，完整挑战以行为契约和实际证据验收。开放设计不设唯一答案。
