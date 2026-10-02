# H06 — 尝试后的参考

默认授课路径不读取本文件。明确请求完整参考时再使用；参考实现不记录为独立完成。

最小参考组件：components/status_policy/CMakeLists.txt 登记 status_policy.cpp 和 include，main REQUIRES status_policy。不要把 REQUIRES/PRIV_REQUIRES 当作“高级/低级”，选择依据是消费者是否需要该依赖。Controller 通过构造注入 HAL 引用，host 测试传 fake，实机传 ESP32 实现。

Guided 示例位于 examples/，完整挑战以行为契约和实际证据验收。开放设计不设唯一答案。
