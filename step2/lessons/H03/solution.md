# H03 — 尝试后的参考

默认授课路径不读取本文件。明确请求完整参考时再使用；参考实现不记录为独立完成。

单位 Bug 修复：1 秒周期使用 1000000 μs。消抖参考算法见 STEP 1 自己已通过的实现；迁移时以 esp_timer_get_time()/1000 作为真实时钟，保持上电状态与“松开后允许触发”约定。计时检查 now >= deadline，等待只用短超时；若同一轮取消与到期同时发生，必须明确定义优先规则并测试。

Guided 示例位于 examples/，完整挑战以行为契约和实际证据验收。开放设计不设唯一答案。

## 可构建的参考策略

完整参考代码：[button_timer.hpp](solution/button_timer.hpp)。上电按住不启动、稳定 30 ms、
按住不重复、5 s 到期和同轮取消优先在维护者 host_contracts 中实际测试。
实机适配：GPIO raw==0 转 pressed，esp_timer_get_time()/1000 转 now_ms，update 的 Event
交给现有 owner 更新 LED/协议。必须使用单调时间；该策略不包含 GPIO Driver，不替代实物验收。
