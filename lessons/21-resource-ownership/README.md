# Lesson 21：Mutex、Semaphore 与何时不用 RTOS

按钮请求由输入端产生，网络端发送；共享状态必须有清晰所有权。确定性模型展示一次按下只产生一个通知、离线时保留消息、故障时停止发送。`%concurrency_probe` 的真实线程只验证 host 的互斥与 join。

本课最后问一个设计问题：只有一个周期循环、两个很短的动作时，为什么状态机往往比 Task/Mutex 更容易证明正确？FreeRTOS 的 Task、Queue、Mutex、Semaphore、Event 和 Timer 会在 STEP 2 映射到 ESP-IDF API，并单独验证栈、优先级、调度和 watchdog。
