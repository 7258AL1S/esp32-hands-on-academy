# H02 — 尝试后的参考

默认授课路径不读取本文件。明确请求完整参考时再使用；参考实现不记录为独立完成。

漏登记的完整修复：idf_component_register(SRCS "main.cpp" "device_name.cpp" INCLUDE_DIRS "." REQUIRES freertos log)。模板根文件加载 project.cmake 后调用 project(desk_tool)。按钮练习的参考设计是 app_main 组装依赖、Controller 只调用 Board、Esp32Board 管理引脚；以自己的 STEP 1 代码为迁移起点。

Guided 示例位于 examples/，完整挑战以行为契约和实际证据验收。开放设计不设唯一答案。
