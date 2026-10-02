# ESP32 Hands-on Academy

面向中文 Maker 的 ESP32 实践式学习环境。

你不需要先买开发板，也不需要先学 Python。课程默认使用 **VS Code Notebook + 可编辑 C++ 代码格 + 可交互虚拟实验台**：先在电脑上运行、修改、故意弄坏并调试 C++，再把相同的设计迁移到真实 ESP32。

## 你会得到什么

课程的目标不是背 API，而是逐步做到：

```text
看懂 → 跑起来 → 修改 → 故意弄坏 → 调试 → 独立设计 → 完成实用项目
```

第一阶段（STEP 1）不需要 ESP32 硬件。电脑上的实验台会显示按钮、LED、ADC、PWM 波形、传感器、UART、I2C、SPI、网络报文、任务队列和检查卡片。你的输入会进入当前代码格编译出的 C++ 程序，面板显示程序的实际输出。

第二阶段（STEP 2）再把同一套 Controller、状态机、协议和测试迁移到 GPIO、Timer、Interrupt、Wi-Fi、FreeRTOS、BLE 和 OTA。电脑模拟器不会冒充真实硬件验证：电气参数、无线射频、芯片时序、Flash 分区和真实 FreeRTOS 调度仍要在硬件上检查。

## 开始前：电脑要求

### 最低要求

- macOS、Windows 10/11 或 Linux；
- 4 GB 以上可用内存，建议 8 GB；
- 能联网安装 VS Code 扩展和 Python 依赖；
- Python 3.10 或更新版本，以及 Git；
- C++17 编译器和 CMake 3.16 或更新版本；
- STEP 1 不需要 ESP32 开发板。

课程代码以 C++ 为主。Python 只负责启动 Jupyter kernel、注册 Notebook magic 和显示 ipywidgets 面板；你不需要编写 Python。

## 第 0 步：安装 Python 和 Git

如果电脑已经有 Python 3 和 Git，可以直接检查版本：

```sh
python3 --version
git --version
```

Windows PowerShell 使用：

```powershell
py --version
git --version
```

如果没有安装：

- macOS：安装 [Homebrew](https://brew.sh/)，然后运行 `brew install python git`；
- Windows：安装 [Python](https://www.python.org/downloads/windows/) 和 [Git for Windows](https://git-scm.com/download/win)，安装 Python 时勾选 **Add Python to PATH**；
- Ubuntu / Debian：运行 `sudo apt update && sudo apt install python3 python3-venv python3-pip git`。

课程使用项目自己的虚拟环境，因此不会修改系统 Python 的第三方包。

## 第 1 步：安装 VS Code

从官方页面安装 Visual Studio Code：

<https://code.visualstudio.com/>

安装后启动 VS Code。后面的所有操作都在 VS Code 中完成，不要直接双击 Notebook 文件。

## 第 2 步：安装 VS Code 扩展

打开左侧 **Extensions / 扩展**（快捷键 `⇧⌘X`，Windows/Linux 使用 `Ctrl+Shift+X`），搜索并安装：

1. **Python**（扩展 ID：`ms-python.python`）
2. **Jupyter**（扩展 ID：`ms-toolsai.jupyter`）

建议同时安装：

3. **C/C++**（扩展 ID：`ms-vscode.cpptools`），用于 C++ 语法高亮和诊断；
4. **CMake Tools**（扩展 ID：`ms-vscode.cmake-tools`），用于查看 CMake 项目；课程命令本身不依赖它。

项目中的 `.vscode/extensions.json` 会自动推荐前两个扩展。看到推荐提示时选择 **Install** 即可。

## 第 3 步：安装 C++ 编译器和 CMake

### macOS

打开 **Terminal / 终端**：

```sh
xcode-select --install
```

如果系统提示已经安装，可以跳过。然后安装 CMake：

```sh
brew install cmake
```

如果电脑没有 Homebrew，先按 <https://brew.sh/> 的说明安装，或者从 <https://cmake.org/download/> 安装 CMake。

检查安装结果：

```sh
clang++ --version
cmake --version
```

### Windows

安装以下两项：

1. **Visual Studio Build Tools**：<https://visualstudio.microsoft.com/visual-cpp-build-tools/>
2. **CMake**：<https://cmake.org/download/>

安装 Visual Studio Build Tools 时勾选 **Desktop development with C++**，并保留 MSVC、Windows SDK 和 CMake 工具。之后使用 **Developer PowerShell for VS** 或普通 PowerShell 检查：

```powershell
cl
cmake --version
```

如果 `cl` 只在 Developer PowerShell 中可用，请从该终端启动 VS Code：

```powershell
code .
```

### Ubuntu / Debian Linux

```sh
sudo apt update
sudo apt install build-essential cmake
g++ --version
cmake --version
```

其他发行版请安装对应的 GCC/Clang、CMake 和 Python 3 包。

## 第 4 步：获取项目

### 使用 Git 克隆

```sh
git clone https://github.com/7258AL1S/esp32-hands-on-academy.git
cd esp32-hands-on-academy
```

如果你从 ZIP 下载项目，解压后进入包含 `README.md`、`notebooks/` 和 `tools/` 的目录。

### 在 VS Code 打开正确的目录

在项目目录执行：

```sh
code .
```

也可以在 VS Code 中选择 **File → Open Folder...**，打开 `esp32-hands-on-academy` 文件夹。

请确认 VS Code 的 Explorer 顶层能看到：

```text
README.md
notebooks/
lessons/
tools/
CMakeLists.txt
```

不要只打开某个 `.ipynb` 文件，也不要打开整个 `Documents` 目录，否则 Notebook 可能找不到项目工具。

## 第 5 步：创建 Python 环境

Python 只作为 Notebook 运行支架。建议使用项目自己的 `.venv`，不要把依赖安装到系统 Python。

### macOS / Linux

在 VS Code 的 **Terminal → New Terminal** 中运行：

```sh
python3 -m venv .venv
.venv/bin/python -m pip install --upgrade pip
.venv/bin/python -m pip install -r requirements-notebook.txt
```

### Windows PowerShell

```powershell
py -m venv .venv
.venv\Scripts\python -m pip install --upgrade pip
.venv\Scripts\python -m pip install -r requirements-notebook.txt
```

依赖包括 Jupyter kernel、Notebook 执行器、`ipywidgets` 和 Notebook 格式工具。安装完成后重启 VS Code，确保扩展发现新的 `.venv`。

## 第 6 步：选择 Notebook Kernel

1. 在 Explorer 打开 `notebooks/01-smart-button.ipynb`；
2. 点击 Notebook 右上角 **Select Kernel**；
3. 选择 **Python Environments**；
4. 选择项目中的 `.venv`：
   - macOS/Linux：`.venv/bin/python`
   - Windows：`.venv\Scripts\python.exe`

如果列表没有 `.venv`，选择 **Select Another Kernel... → Python Environments...**，或者运行命令面板中的 **Python: Select Interpreter**，先选择 `.venv`。

第一次显示交互控件时，VS Code 可能提示启用 `@jupyter-widgets/base` 或 `@jupyter-widgets/controls`。这是 Jupyter 的标准前端资源，请选择 **Enable / 启用下载**。

## 第 7 步：运行第一课

打开 [Lesson 01 Notebook](notebooks/01-smart-button.ipynb)，按顺序运行代码格：

1. 点击第一个初始化代码格左侧的 **▶**；
2. 等待出现“C++ 实验格已准备好”；
3. 继续运行板卡介绍格，观察 ESP32-DevKitC 的功能分组和针脚；
4. 运行 C++ 实验格，等待 CMake 编译完成；
5. 在深色实验台中点击 **按下按钮 / 松开按钮**；
6. 点击 **采样 +10 ms**，观察按钮状态和 LED 的真实 C++ 输出；
7. 打开 **连续采样（慢放）**，观察时间变化；
8. 点击 **播放自动检查**，查看绿色通过或红色失败卡片。

运行 Notebook 的快捷键：

- `Shift+Enter`：运行当前格并进入下一格；
- `Ctrl+Enter`（macOS 为 `⌘Enter`）：运行当前格并停留；
- `Esc` 后按 `0`、`0`：重启 kernel；
- 重启 kernel 后，必须重新运行初始化格。

### 面板中的控件代表什么

- **按下按钮 / 松开按钮**：改变 Virtual GPIO 输入；
- **采样 +10 ms**：推进 10 ms 虚拟时间并调用一次 C++ `tick()`；
- **连续采样（慢放）**：每隔一小段真实时间自动推进虚拟时间；
- **复位**：重建 C++ Controller，清除状态和首次失败记录；
- **播放自动检查**：用当前代码执行一组可重复场景；
- **红色卡片**：显示失败场景、预期、实际和首次发生条件；
- **波形 / 终端 / 网络报文**：来自当前代码的实际观测，不是预先录制的动画。

## 第 8 步：按正确顺序学习一课

每课都尽量遵循以下顺序：

1. **Guided**：先运行完整示例，观察问题和输出；
2. **Modify**：只改一个参数或一小段逻辑；
3. **Broken / Debug**：运行故意有问题的代码，先看红色证据；
4. **Build**：只根据需求、约束和分层 Hint 自己实现；
5. **Solution**：完成尝试后，再打开独立答案 Notebook 对照。

课程代码格中的 `%%academy_lab`、`%%network_lab` 和 `%concurrency_probe` 是已经注册好的 Notebook 命令。它们不是 Python 课程内容；只要按顺序运行即可。

默认不会把 Solution 混入主课，自动测试检查的是你当前代码格编译出的程序。答案位于 `notebooks/solutions/`，不要一开始就打开。

## 课程路线

| Lesson | 主题 | 入口 |
| --- | --- | --- |
| 01 | ESP32 板卡、针脚、电路基础、按钮与 LED | [01](notebooks/01-smart-button.ipynb) |
| 02 | 按键抖动与 debounce | [02](notebooks/02-reliable-button.ipynb) |
| 03 | 非阻塞 Timer 与倒计时 | [03](notebooks/03-nonblocking-timer.ipynb) |
| 04 | State Machine | [04](notebooks/04-state-machine.ipynb) |
| 05 | C++ 资源、内存、模块化与错误边界 | [05](notebooks/05-embedded-cpp.ipynb) |
| 06 | PWM 调光 | [06](notebooks/06-pwm-dimmer.ipynb) |
| 07 | ADC、分压、量化与校准 | [07](notebooks/07-adc-sensor.ipynb) |
| 08 | Driver 与 HAL | [08](notebooks/08-driver-hal.ipynb) |
| 09 | Polling、Interrupt 与事件计数 | [09](notebooks/09-interrupts.ipynb) |
| 10 | UART 字节流与帧 | [10](notebooks/10-uart-terminal.ipynb) |
| 11 | I2C 地址、ACK 与超时 | [11](notebooks/11-i2c-sensor.ipynb) |
| 12 | SPI、CS 与显示事务 | [12](notebooks/12-spi-display.ipynb) |
| 13 | TCP 状态服务 | [13](notebooks/13-tcp-status-service.ipynb) |
| 14 | UDP 发现 | [14](notebooks/14-udp-discovery.ipynb) |
| 15 | HTTP、REST 与 JSON | [15](notebooks/15-http-rest-json.ipynb) |
| 16 | Wi-Fi 状态、DNS 与连接恢复 | [16](notebooks/16-connection-state-and-dns.ipynb) |
| 17 | WebSocket 实时状态 | [17](notebooks/17-websocket-live-status.ipynb) |
| 18 | MQTT、Topic 与 QoS 0 | [18](notebooks/18-mqtt-reminder.ipynb) |
| 19 | Task、Producer / Consumer 与 Queue | [19](notebooks/19-concurrent-tasks.ipynb) |
| 20 | Synchronization、Race 与故障状态 | [20](notebooks/20-sync-and-race.ipynb) |
| 21 | Resource ownership 与消息发送 | [21](notebooks/21-resource-ownership.ipynb) |
| 22 | Persistent config 与版本迁移 | [22](notebooks/22-config-migration.ipynb) |
| 23 | BLE provisioning 服务模型 | [23](notebooks/23-ble-provisioning.ipynb) |
| 24 | OTA 校验、健康检查与回滚 | [24](notebooks/24-safe-ota.ipynb) |
| 25 | 独立项目：设计一个真正有用的小工具 | [25](notebooks/25-capstone-project.ipynb) |

完整课程规划、STEP 2 迁移单元和项目 Track 见 [CURRICULUM.md](CURRICULUM.md)。

## 常用验证命令

这些命令供维护者或想检查环境的学习者使用。它们不会替代 Notebook 学习路径。

### 检查 Notebook 和工具

macOS/Linux：

```sh
.venv/bin/python tools/run_notebook_smoke.py
.venv/bin/python tools/verify_panels.py
```

Windows PowerShell：

```powershell
.venv\Scripts\python tools\run_notebook_smoke.py
.venv\Scripts\python tools\verify_panels.py
```

### 单独构建某课的 C++ solution

```sh
cmake -S . -B .build/lesson02 \
  -DACADEMY_LESSON=02-reliable-button \
  -DLAB_VARIANT=solution
cmake --build .build/lesson02 --parallel
./.build/lesson02/controller_tests exercise
```

### 网络课程验证

```sh
.venv/bin/python tools/network_verify.py
```

网络测试使用真实 localhost socket；它不会证明 Wi-Fi 射频或真实 ESP32 已连接。

## 常见问题

### `Cell magic %%academy_lab not found`

先运行 Notebook 最上面的初始化格。重启 kernel 后也必须重新运行初始化格。Lesson 01 使用 `%%cpp_lab`，其他外设课使用 `%%academy_lab` 或网络专用 magic。

### 控件只显示文字，按钮不能点击

确认：

1. Notebook 使用的是项目 `.venv` kernel；
2. 初始化格已经成功运行；
3. VS Code 已允许下载 Jupyter widget 前端资源；
4. 代码格已经重新执行，而不是查看旧的保存输出。

仍然无响应时，在命令面板执行 **Developer: Reload Window**，重新选择 `.venv`，再从初始化格开始运行。

### `找不到 CMake` 或 C++ 构建失败

在 VS Code Terminal 执行：

```sh
cmake --version
clang++ --version   # macOS
g++ --version       # Linux
```

Windows 请在 Developer PowerShell 中执行 `cl`。修复工具链后重启 VS Code，再重跑当前代码格。编译器完整诊断会显示在代码格下方。

### 修改后输出没有变化

请重新运行**修改后的那一个代码格**。每个代码格会单独写入 `.build/notebook/<lesson>/<name>/source/` 并重新编译；上一个代码格的 Controller 状态不会自动传给下一个代码格。

### 按住按钮后出现红色失败

这通常是故意保留的 Debug Challenge：先读清楚红色卡片中的预期、实际和发生时间，再看 `lessons/<lesson>/hints.md`。不要直接把它当作 Notebook 或 kernel 故障。

### 为什么电脑模拟通过后还要买板验证

Host 模拟验证的是接口和软件行为。它不验证 GPIO 电气安全、真实按键抖动、ADC 误差、UART 电平、I2C 上拉、SPI 时序、Wi-Fi 射频、BLE 配对、FreeRTOS 栈和 OTA Flash 分区。STEP 2 会为每个迁移单元提供 Hardware Checklist。

## STEP 2：拥有 ESP32 后如何迁移

STEP 2 不是把课程重新学一遍，而是替换硬件层：

```text
Virtual GPIO / Virtual Timer
        ↓
Board / HAL contract
        ↓
ESP32 GPIO / Timer / Driver
        ↓
真实 LED、Button、Sensor、Display 和网络
```

开始接线前，确认具体 ESP32 型号、官方 pinout、3.3 V 电平、GND、外部 LED 限流电阻和输入上拉/下拉。经典 ESP32、ESP32-S3、ESP32-C3 的 GPIO 能力和针脚并不完全相同。

课程设计参考：

- [ESP32-DevKitC 官方指南](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp32-devkitc/user_guide.html)
- [经典 ESP32 数据手册](https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf)

Arduino 与 ESP-IDF 的选择会在硬件迁移阶段结合项目需求判断，不把“更专业”当作唯一标准。

## 项目结构

```text
notebooks/                       主课程 Notebook
notebooks/solutions/             独立参考答案
lessons/<lesson>/                README、starter、challenge、solution、tests、hints
include/academy/                 Board、Device、网络和系统抽象
simulator/                       Virtual GPIO / Device host runtime
tools/                           Notebook、交互面板和验证工具
final-projects/                  最终项目 Track 与验收要求
ARCHITECTURE.md                  技术架构和 STEP 1 → STEP 2 迁移方式
CURRICULUM.md                    总课程计划
UI_GUIDELINES.md                 全课程深色 UI 规范
```

## 贡献与许可

欢迎提交中文解释、实验改进、可访问性修复、跨平台修复和真实硬件迁移经验。请先阅读 [CONTRIBUTING.md](CONTRIBUTING.md)。新增实验应复用 [UI_GUIDELINES.md](UI_GUIDELINES.md)，并提供可运行代码、失败场景和自动验证。

本项目使用 [MIT License](LICENSE)。
