# Lesson 19：并发任务从问题开始

网络发送偶尔阻塞时，传感器采样和按钮响应是否还应该停止？本课用确定性的 `tick()` 模型拆开 sensor/network/input 三个职责。`output.queue_depth` 是可观察的 producer/consumer 队列；它不是 FreeRTOS Queue。

Notebook 中的 `%%academy_lab 19-concurrent-tasks guided pass` 会编译当前 C++。`%concurrency_probe` 另外运行受限的真实 `std::thread`，帮助比较 host 线程和 ESP-IDF FreeRTOS Task 的边界。

先运行 guided，再修改采样周期和离线恢复逻辑，最后打开 challenge 制造“网络离线导致采样停止”的故障。答案在 `solution/`。
