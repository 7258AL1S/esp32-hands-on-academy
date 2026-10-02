# H13 — 尝试后的参考

默认授课路径不读取本文件。明确请求完整参考时再使用；参考实现不记录为独立完成。

无条件确认 Bug 修复：把 mark_app_valid_cancel_rollback 放到真实自检成功分支，失败触发回滚。OTA 下载 ESP_OK 不等于新固件已运行；复位后核对 running partition 与版本。启用 rollback 的 bootloader 和新分区首次需串口部署；不要只 OTA app 然后期待旧 bootloader 支持新策略。

Guided 示例位于 examples/，完整挑战以行为契约和实际证据验收。开放设计不设唯一答案。
