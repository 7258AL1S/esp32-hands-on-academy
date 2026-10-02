# Lesson 20：Queue、Event 与 race

设备离线时脉冲事件不能丢失，恢复后消费者一次处理一条。先用 `pulses` 输入观察确定性队列模型，再用 `%race_probe` 看真实 `std::thread` + `std::atomic`。host probe 不是 FreeRTOS 调度器，也不模拟中断上下文。

故障挑战会覆盖“新事件覆盖旧 backlog”的错误。不要运行无界死锁样例；通过固定锁顺序、有限超时和清晰资源所有权来调试并发系统。
