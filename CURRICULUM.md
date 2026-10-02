# ESP32 Hands-on Academy — 总课程计划

**状态：STEP 1 Lesson 01–25 已实现第一版。** 现在进入整体验证和学习反馈阶段；STEP 2 的硬件迁移清单与适配器按真实硬件逐模块开展，不把 host 通过包装成硬件通过。

## 学习形式与成长线

面向已有一定 C++ 基础、没有 ESP32 实战经验的学习者。正文简体中文，实验 C++，Python 只承担 Notebook 支架。已掌握的 C++ 内容通过短挑战跳过。

每课：真实问题 → 最小知识 → 可交互外设 → 运行/修改 C++ → 注入故障 → 动态检查 → 独立挑战。Hint 和答案分开。贯穿项目是**桌面状态与提醒工具**：按钮/灯 → 定时 → 传感器/显示 → 通信 → 网络 → 并发 → 可配置、可升级设备。

第一课先认识板卡与必需电路知识，同时用按钮引起兴趣；完整理论按需要出现，不先安排长篇电工课。

## STEP 1 — 没有硬件，在 VS Code 运行

| 阶段 / 课号 | 问题与知识 | 直观实验 | 能力验收 |
|---|---|---|---|
| 入门 / 01 | 提醒按钮；芯片/模组/板卡、针脚、3V3/GND、回路、上拉、限流、level/edge | 针脚探索、电阻调节、按钮/LED、单步/慢放、红绿动画检查 | 解释输入到输出，修复长按重复触发 |
| 可靠输入 / 02 | 一次按下为何触发多次；浮空、debounce、RC 的作用 | 抖动注入、输入波形、稳定时间滑块 | 定义可靠按键事件 |
| 时间 / 03–04 | 等待期间无法响应；blocking delay、单调时钟、Timer、超时、状态机 | 时间步进、倒计时、多灯与事件时间线 | 用非阻塞状态机组织工具 |
| 程序与资源 / 05 | 模块越来越多；struct/enum、pointer/reference、bit manipulation、callback/function pointer、header/source、编译/链接、错误返回、stack/heap、静态/动态内存 | 缓冲区与资源占用、越界/错误挑战；已有基础可跳过 | 模块化并解释资源边界 |
| 外设与电路 / 06–08 | 调亮度、读旋钮和传感器；PWM、ADC、分压、量化、校准、Driver/HAL | LED 亮度、PWM 波形、电压/旋钮、噪声/断线、仪表 | 设计可测驱动，区分模拟值和数字电平 |
| 响应 / 09 | 短脉冲漏读；Polling、Interrupt、事件、ISR、Watchdog | 短脉冲注入、轮询/中断时间线、卡死指示 | 按延迟和复杂度选择机制 |
| 设备通信 / 10–12 | 与电脑、传感器和显示屏通信；UART、帧、缓冲、I2C、地址/ACK、SPI、片选 | 串口终端、虚拟温度/光照、小显示屏、总线错误 | 选择接口并处理超时、设备缺失和不完整数据 |
| 网络主线 / 13–16 | 电脑/手机查看状态；Client/Server、IP/端口、TCP/UDP、HTTP/REST、JSON、DNS/mDNS、Wi-Fi 状态 | 真实 host socket/API、浏览器控制、断连注入 | 建立状态 API，定位连接和协议问题 |
| 实时与消息 / 17–18 | 页面实时更新、多设备通知；WebSocket、MQTT、Broker/Topic、QoS、重连、Device/Gateway | 本机真实通信、消息流、断网/恢复 | 按需求选择网络协议 |
| 并发与系统 / 19–21 | 网络、传感器、按钮相互阻塞；Task、Queue、Semaphore、Mutex、Event、软件 Timer、同步、producer/consumer、race/deadlock | 任务泳道、队列、资源与锁等待、故障注入 | 判断何时不需要 RTOS，划分任务和资源所有权 |
| 配置与部署 / 22–24 | 保留设置、发现/配置设备、升级失败恢复；持久化、配置版本、provisioning、BLE 服务模型、OTA | 配置存储、BLE 事件模型、版本/升级状态图 | 设计配置、恢复与更新流程 |
| 独立项目 / 25 | 从提醒器变成自己需要的工具 | 自定需求、模块、协议和界面，做故障演练 | 从需求、设计、测试到部署方案完成系统 |

课号与拆分随体验调整，验收按能力，不按阅读页数。

## 电路基础安排

- **第一课实验前**：电压/电流/电阻与回路，电源/信号区别，GPIO 编号，3.3 V 逻辑，共地，LED 极性与限流，上拉与浮空。先做少量计算和交互。
- **去抖和采样时**：电容、RC、充放电、采样间隔，再比较硬件与软件去抖。
- **ADC 与传感器时**：分压、量化、噪声和校准；理想计算与实测分开。
- **需要更大负载时**：电流能力、晶体管/MOSFET、独立电源；电感负载再引入续流保护。GPIO 不直接驱动大电流负载。
- **STEP 2 上电前**：真实原理图/pinout、万用表、接线极性、供电方式和器件额定值。教学示意图不能当作排针孔位图。

## STEP 2 — 逐模块迁移到真实 ESP32

每个迁移单元带上 STEP 1 的 C++ 逻辑、测试、接口契约与 Hardware Checklist；不重新学一遍。

| 单元 | 保留的设计 | 替换的硬件层 | 真实验证 |
|---|---|---|---|
| H01 第一个 I/O | 按键 Controller 和事件契约 | Esp32Board、GPIO、实际 pinout | 上电/烧录/串口、外部 LED、接线、复位时按住 |
| H02 时间/PWM/ADC | 去抖、状态机、采样与校准接口 | Timer、Interrupt、PWM、ADC | 时间误差、抖动、模拟电压、频率/占空比 |
| H03 外部设备 | 帧解析、驱动协议、显示模型 | UART/I2C/SPI HAL 与器件 | 波特率、电平、地址、ACK、实际读值 |
| H04 网络工具 | API、协议、重连、错误处理 | Wi-Fi driver、DNS/mDNS、网络栈 | AP/IP、电脑/手机互通、断连、MQTT |
| H05 RTOS | 职责、消息和资源所有权 | 真正 ESP-IDF FreeRTOS 的 Task/Queue/Mutex 等 | Task 栈、优先级、调度、负载、watchdog |
| H06 配置/更新 | 配置版本、状态机、恢复流程 | NVS、BLE、provisioning、OTA 分区 | 配网、发现/配对、掉电、升级失败/回滚 |
| H07 个人项目 | 既有需求、设计和测试 | 实际部署与封装 | 供电/接线、现场网络、长时间运行、文档 |

**Arduino 与 ESP-IDF** 在 H01 用小适配器比较：`setup/loop` 与 `app_main`、组件构建、驱动接口。H04–H06 随项目需要选择，不用“谁更专业”代替判断。购买前核对 ESP32/S3/C3 等型号能力和引脚。

## 模拟的实施策略与边界

- **已实现**：针脚探索、LED 限流近似计算、按钮/LED 交互、持久 C++ 进程、单步/慢放、复位、动态检查、错误标红；ADC/传感器滑块、噪声/断线、PWM 波形、UART 终端、I2C/SPI 事务、任务/队列视图。
- **网络已实现**：TCP、UDP、HTTP/REST/JSON、localhost/DNS 状态、WebSocket、MQTT QoS 0 MiniBroker，所有协议课通过真实 localhost socket 验证。共用“输入控件 → HAL → 学生 C++ → 观测 → 契约检查”管线。
- **网络**：TCP/UDP/HTTP/WebSocket/MQTT 尽量用真实本机通信；Wi-Fi 射频和 AP 关联用明确标注的状态模型，host 不能证明无线连接。
- **BLE**：STEP 1 模拟服务/特征和连接事件，真实无线发现/配对在 STEP 2 验证。
- **FreeRTOS**：host 并发模型先暴露同步问题，明确它不是真正 FreeRTOS scheduler；条件允许时增加真实 host port 选修。ESP-IDF 调度、栈和中断行为在硬件验证。
- **OTA**：host 练习版本、校验和失败恢复；分区、bootloader、Flash 写入和回滚在 ESP32 验证。

## 最终项目与验收

| Track | 小工具例子 | 重点 |
|---|---|---|
| Networked Tool | 网络状态灯、Wi-Fi 诊断工具、物理通知按钮 | 协议、重连、可观测性 |
| IoT Device | 环境监视器、Web/MQTT dashboard | 采样、校准、数据质量、通信 |
| Smart Utility | 智能计时器、信息显示、电脑物理接口 | 输入、状态机、易用性、配置 |
| Embedded System | 串口桥、通信监视器、可升级设备 | 驱动、资源、RTOS/事件、维护 |
| 可选 Control | 控制/机器人接口扩展 | 仅作应用方向，不作前提 |

统一验收：需求/约束 → 系统图/模块 → 协议选择 → 状态/任务设计 → 实现 → 自动测试 → 故障与 Debug → 硬件清单 → 文档 → 部署/适用的 OTA。无唯一答案，按自己定义的需求证明可靠性。

## 当前迭代闸门

Lesson 01 的深色 UI 约定已经固化到 [UI_GUIDELINES.md](UI_GUIDELINES.md)，并用于 Lesson 02–25。下一轮根据学习者实际体验调整顺序、提示和难度；修改课程体验时仍需运行对应 Notebook smoke、行为测试和 UI 检查。

资料：本课板卡介绍已核对 [Espressif ESP32-DevKitC V4 官方指南](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp32-devkitc/user_guide.html)。GPIO 电气参数以 [经典 ESP32 数据手册](https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf) 为准。未来各 Driver/Framework/FreeRTOS Lesson 单独固定版本并核对官方资料；计划中的未实现 API 不声称已验证。
