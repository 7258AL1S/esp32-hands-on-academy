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

## STEP 2 — 迁移硬件，学会独立建工程

**状态：规划，尚未实现硬件 Lesson 或完成真实开发板验收。** 每个单元复用 STEP 1 的控制逻辑、测试和接口设计，并加入真实硬件与工程实践。完整的实验安排、支架退出和验收要求见 [STEP2_PLAN.md](STEP2_PLAN.md)。

| 单元 | 内容 | 能力验收 |
|---|---|---|
| H00 环境与设备连接 | ESP-IDF 扩展、工具链、芯片/串口、构建、烧录、启动日志 | 独立跑通官方最小固件，区分构建/烧录/启动错误 |
| H01 第一次 I/O 迁移 | 真实 GPIO、LED、Button；供电、极性、限流；Arduino / ESP-IDF 对照 | 迁移按键事件逻辑，验证实物与串口状态 |
| H02 独立建工程 I | 新目录、官方模板、C++ 入口、源文件登记、CMake、target、build/flash/monitor | 自己新建工程并添加模块，修复源文件漏登记问题 |
| H03 时间与 Interrupt | debounce、Timer、非阻塞状态机、ISR、watchdog、复位诊断 | 实际计时仍能响应输入，并用证据定位故障 |
| H04 PWM、ADC 与测量 | 调光、电位器、分压、采样和校准 | 比较理论与实测，处理误差和输入边界 |
| H05 外部设备通信 | UART、I2C；SPI 显示扩展；帧、地址、超时、断线 | 选择接口，完成真实事务与故障恢复 |
| H06 独立建工程 II | Controller/HAL/Driver 组件、依赖、配置、日志和调试 | 独立维护工程，复用 host 测试，解释编译/链接/运行错误 |
| H07 Wi-Fi 与设备发现 | AP、IP、断连/重连、DNS、mDNS | 在真实局域网找到并访问设备 |
| H08 HTTP 与局域网工具 | 状态 API、浏览器控制、JSON、UDP 发现 | 外部客户端读写真实设备，定位协议问题 |
| H09 WebSocket 与 MQTT | 实时状态、Broker、Topic、订阅与恢复 | 按需求选择协议，完成断连故障演练 |
| H10 FreeRTOS 与资源 | Task、Queue、同步、资源所有权、栈和优先级 | 在负载下验证响应，并判断何时保留单一事件循环 |
| H11 NVS 与设备配置 | 持久化、版本迁移、输入校验和出厂恢复 | 重启保留配置，有明确恢复路径 |
| H12 BLE 配置工具 | 真实广播、Service、Characteristic、provisioning | 发现设备、校验配置、处理连接/断开与凭据 |
| H13 OTA 与部署恢复 | 分区、候选固件、启动确认、回滚和版本 | 真实更新与失败恢复，保留串口救援方式 |
| H14 独立项目与交接 | 在课程目录之外从模板建工程，自定需求、设计、测试和文档 | 按自己的 README 在干净环境构建烧录，脱离 Notebook 运行和诊断 |

**独立开发贯穿三次验收：H02 新建工程 → H06 组件与调试 → H14 离开教程。** 前期 Notebook 辅助运行与观察，中段以正常 `.cpp` / `.hpp` 为代码主体，后期使用官方工具独立构建与烧录。每个自动化入口都要解释对应的官方命令。

工程主线采用 ESP-IDF + VS Code ESP-IDF 扩展；Arduino 在 H01 对照，并作为选修或最终项目可选路线。这个选择用于练习组件、FreeRTOS、NVS 和 OTA，不以“更专业”替代需求判断。具体工具链、芯片和引脚在 H00 实施时固定并核验。

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
