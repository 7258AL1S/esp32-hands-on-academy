# STEP 2 — 从硬件迁移到独立开发

**状态：H00–H14 Notebook Alpha 已实现，真实开发板验收与完整 agent 授课体验仍待完成；具体检查见 [step2/VALIDATION.md](step2/VALIDATION.md)。** H00–H14 是学习单元；外设、网络等较大的单元会拆成多个小实验，不要求一次完成全部功能。

目标：把 STEP 1 的桌面状态与提醒工具迁移到真实 ESP32，并能够在新的目录中，使用 VS Code 和官方工具链独立创建、维护、烧录和调试一个工程。

## 课程形式与支架退出

默认学习入口是 Codex VS Code 侧边栏。每课 Notebook 开头提供启动 prompt，agent 读取
当前步骤并逐步授课，Notebook 保持人工可学。步骤带 Guided/Modify/Debug/Build/Open
metadata，Hint 与 Solution 隔离，规则见 [step2/TEACHING.md](step2/TEACHING.md)。

H00–H01 使用引导工程；H02 建立没有课程功能的最小工程，解释入口/CMake/配置。
H03–H14 全程扩展同一工程，以 Git 保存起点，不每课新建工程。示例增量合并、保留
已有功能与唯一 app_main；工程 .cpp/.hpp 是唯一运行源码。Notebook C++ 格只预览示例，
不会暗中改工程或烧录；所有构建/烧录/日志使用明确的官方命令。

`.academy/course.json` 链接课程，`AGENTS.md` 约定老师行为，`.academy/progress.md`
保存步骤、提示级别、证据和起点。新聊天读当前源码，不能只信历史完成标记。
环境故障主动排查并解释，挑战先给分层提示。H14 clone/导出同一工程到干净目录，
关闭 Notebook/agent 后按自己的 README 复现，固件不依赖 Python 或课程目录。

| 阶段 | 课程帮助 | 学习者承担 |
|---|---|---|
| H00–H01 | 环境/接线与 Guided 示例 | target/port、构建/烧录/日志和电路检查 |
| H02–H05 | 最小骨架、当前步骤提示 | 工程创建、源码登记、外设与上层行为 |
| H06–H13 | Component 与机制指导，按需故障排查 | 模块/资源/配置/协议/部署设计与验证 |
| H14 | 需求与验收，提示逐渐退出 | 有用工具、故障演练、干净目录复现和交接 |

## Framework 与版本

正式工程主线采用 ESP-IDF + VS Code ESP-IDF 扩展，因为课程需要练习官方工程结构、组件依赖、FreeRTOS、NVS 和 OTA。H01 用同一个简单 Controller 比较 Arduino 的 `setup()/loop()` 与 ESP-IDF 的 `app_main()`，根据库支持、需求和维护方式判断取舍。

选修或最终项目可以选择 Arduino；选择其他 Framework 后仍要记录工具链、依赖和构建流程。H02 的官方 ESP-IDF 工程练习为必修，用来建立最低限度的独立开发能力。

本版固定 ESP-IDF v5.5.1、经典 ESP32-DevKitC V4 / ESP32-WROOM；扩展版本按实际安装记录，尚未宣称跨 OS 验收。H00 记录 OS、开发板型号、芯片、USB 连接方式、串口驱动和引脚表。实际 GPIO、ADC、BLE 与调试方式根据所选板型核对官方资料；目前不预设所有 ESP32 型号具有相同能力。Windows 的硬件路径单独验收 USB、串口和工具链；STEP 1 的 WSL 环境不能自动证明硬件连接可用。

## 单元安排

| 单元 | 要解决的问题与实践 | 验收 |
| --- | --- | --- |
| H00 环境与设备连接 | 安装 ESP-IDF 扩展和工具链；选芯片与串口；构建、烧录官方最小固件，读取启动日志 | 区分编译失败、烧录失败和启动失败；记录板型、工具链与完整操作路径 |
| H01 第一次 I/O 迁移 | 保留 STEP 1 按键事件设计，接入真实 GPIO、外部 LED、Button；学习供电、共地、极性与限流；对照 Arduino / ESP-IDF | 实际灯光、串口状态、按住、松开和上电按住行为一致；完成接线清单 |
| H02 独立建工程 I | 在新目录中从官方模板建项目；将入口改为 C++；登记源文件；理解根目录与组件 `CMakeLists.txt`、`sdkconfig`；移入自己的按钮逻辑 | 使用官方工具完成 target、build、flash、monitor；新增一个模块；修复一次文件未登记造成的构建错误 |
| H03 时间、Interrupt 与故障定位 | 迁移 debounce、非阻塞计时和状态机；比较主循环、Timer、GPIO Interrupt；练日志、复位原因与 watchdog 诊断 | 计时期间按钮可响应；ISR 与主循环职责明确；用观测记录实际误差和故障原因 |
| H04 PWM、ADC 与测量 | 驱动真实 LED 调光，读取电位器；理解占空比、分压、采样、校准和误差 | 测量或观察亮度变化、已知输入与 ADC 读数；区分模型换算与实测结果 |
| H05 外部设备通信 | UART 与电脑交互；I2C 读取一个传感器；SPI 驱动显示器作为扩展；处理半帧、地址、ACK、超时和断线 | 接口选择有依据；断开设备时上层得到错误；恢复连接可继续使用。各接口分实验开展 |
| H06 独立建工程 II：组件与调试 | 拆出 Controller、HAL、Driver 和协议模块；配置 GPIO 与采样参数；登记组件依赖；练编译错误、链接错误和运行错误 | 独立新增组件与配置；复用 host 行为测试；用日志定位问题。支持 JTAG 的板型另做断点调试选修 |
| H07 Wi-Fi 与设备发现 | 连接实际 AP，观察 IP；迁移断连/重连状态机；在真实局域网使用 DNS、mDNS | 电脑能访问设备；断网后有可解释状态，恢复后重新工作；诊断 AP、地址、端口和应用协议 |
| H08 HTTP 与局域网工具 | 在真实设备上部署状态 API 与本地控制页；用浏览器或 `curl` 操作；比较 UDP 发现与 mDNS | 外部客户端读写真实设备状态；检查请求、JSON、状态码、未知命令与重连 |
| H09 WebSocket 与 MQTT | 推送状态变化，连接实际 Broker；选择 Topic 和消息语义；断开服务并恢复 | 接口状态与实际设备一致；解释断连、订阅恢复和消息策略；按工具需求选择协议 |
| H10 FreeRTOS 与资源 | 先测单一事件循环的行为，再按实际需要引入 Task、Queue、Mutex、Semaphore、Event 和软件 Timer | 网络负载下仍可响应按钮；队列容量、资源所有者与同步规则明确；检查栈、优先级和 watchdog |
| H11 NVS 与设备配置 | 保存设备名、阈值和采样间隔；迁移配置版本；输入校验、默认值与恢复出厂设置 | 重启后配置仍在；无效值被拒绝；升级配置后保留有效数据；配置流程有恢复方式 |
| H12 BLE 配置工具 | 选用具备 BLE 的板型；运行真实广播、Service、Characteristic 和配置状态；练连接失败与授权边界 | 手机或电脑发现设备；配置输入被校验；观察连接/断开。凭据不写入公开源码或日志 |
| H13 OTA 与部署恢复 | 配置 OTA 分区；验证包、写候选固件、启动确认和失败回滚；保存设备与固件版本 | 真实设备完成更新与失败恢复；解释完整性校验与身份认证的区别；保留串口重新烧录路径 |
| H14 独立项目与交接 | 自选网络工具、环境监视器、智能计时器、串口工具等；在持续工程完成需求、设计、测试、封装与使用文档，再到干净目录复现 | 课程目录之外可构建烧录；按项目 README 在干净环境复现；提供真实故障记录和硬件验收证据 |

## 独立建工程的三个检查点

### H02：从模板开始

提供等价官方最小工程的空白骨架、需求、接口约定和分层 Hint；亲手创建文件是独立验收，使用复制工具需能解释各文件。学习者自己创建目录，找到程序入口，理解 C++ 中 `extern "C" void app_main()` 的链接约定，将 `.cpp` 加入构建，选择正确 target 和串口，独立执行 build、flash、monitor。

需要能够解释并使用以下操作；占位符按实际芯片和串口替换，命令在配置好的 ESP-IDF Terminal 中执行：

```text
idf.py set-target <chip-target>
idf.py build
idf.py -p <port> flash monitor
idf.py menuconfig
```

区分根目录 `CMakeLists.txt`、组件 `main/CMakeLists.txt`、`main.cpp`、`sdkconfig` 与构建输出；明确源文件登记、配置与编译之间的关系。

挑战：添加 `button_controller.hpp` / `button_controller.cpp`，把已有行为搬入模块；故意漏登记一个源文件，阅读错误并修复。参考工程与练习隔离。

### H06：把小程序变成可维护的工程

围绕已经存在的传感器/显示功能拆模块，避免提前堆出复杂框架。学习组件 `CMakeLists.txt`、依赖、配置、日志和 host 测试入口。能解释哪些文件应进 Git，哪些是生成的构建文件，以及如何记录工具链和依赖版本。

挑战：只根据驱动接口与设备资料添加一个模块，区分 compiler 诊断、linker 诊断和设备运行错误。配置文件与设备凭据分开，私密凭据不提交。

### H14：离开课程支架

继续完成同一工程，使用自己的模块并独立编写 README，包括依赖、板型、引脚、构建、烧录、日志、配置和测试步骤。

验收时在干净目录获取自己的项目，安装记录的工具链，按自己的 README 构建并烧录。关闭 Notebook 后，设备能够独立运行；用官方工具继续读取日志、排查问题和更新固件。项目需要网络或 Broker 时，明确这些运行依赖。

## 每个硬件实验的组织

1. 先描述 STEP 1 已经保证的行为，以及真实硬件带来的一个新问题。
2. 给出该板型的接线图、器件清单和上电检查；分批添加外设。
3. 展示涉及的工程文件和最少 Driver 知识，修改并构建当前实现。
4. 烧录，观察设备日志与真实外设，再修改参数。
5. 制造有边界的软件错误，例如极性判断错误、半帧处理错误、超时或消息丢失；记录现象并调试。
6. 完成分层挑战，分别填写软件检查与 Hardware Checklist，记录迁移后的设计取舍。

Notebook 提供官方命令和示例预览，不自动执行烧录。学习者/agent 在工程终端明确运行、读实际结果，烧录前释放已有 monitor。手动证据卡标明“学习者记录”，不是实物自动测量。到 H02 能够手工执行等价流程。

## 验证边界

检查卡片分别记录：host 行为测试、固件构建、烧录、设备启动、设备协议往返、实物观察。每种失败显示对应错误，不把编译或烧录成功等同于功能成功。

设备上报 `led=ON` 能证明软件上报的输出状态；LED 实际是否发光，需要实物观察或额外测量。PWM 频率、ADC 精度、Interrupt 延迟和电气问题也按所用仪器与观察条件记录证据。

自动检查覆盖可稳定复现的协议与状态；接线、供电、实测值和恢复行为由 Hardware Checklist 补足。H14 项目选择适用的测试与部署方式，OTA 是需要远程更新的项目要求。

## 官方参考入口

- [ESP-IDF VS Code 扩展文档](https://docs.espressif.com/projects/vscode-esp-idf-extension/en/latest/)
- [ESP-IDF Get Started](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/)
- [ESP-IDF Build System](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/build-system.html)

每课另列 v5.5.1 对应官方文档，Guided 示例以本版本真实工具链编译；升级版本或换芯片需要重新构建和实机验证。

## 当前课程入口

[H00–H14 Notebook 索引](step2/README.md) · [从安装开始](step2/SETUP.md) · [基线引脚与电路](step2/BOARD.md)。
