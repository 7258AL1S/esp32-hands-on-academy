# STEP 2：从安装到真实串口

## 1. 准备板和工具

基线：经典 ESP32-DevKitC V4，ESP32-WROOM（至少 4 MB Flash）、数据 USB 线。
兼容板也可用，但须核对芯片/pinout/USB 桥接芯片，不能照排针位置接线。
H00 只需板和线，H01 再准备 LED、330 Ω 电阻、按钮、面包板和跳线。
后续按课准备器件，不要求第一天购买全部。参见 [BOARD.md](BOARD.md)。

## 2. 安装 VS Code 扩展

打开扩展面板，安装 **Espressif IDF**（`espressif.esp-idf-extension`）。
侧边栏授课再安装 **Codex**（`openai.chatgpt`），登录支持的账号；Codex 是可选老师，
课程手工操作和固件运行不依赖它。Notebook 仍使用根 README 已配置的 Python/Jupyter。

使用扩展当前版本的 ESP-IDF 安装/配置向导，按照官方指南安装 **ESP-IDF v5.5.1**
和对应工具。部分扩展版本通过 ESP-IDF Installation Manager 安装；菜单名称有差异，
不要只按旧截图找按钮。保留扩展版本与 Doctor 输出。

- macOS：先确保 Xcode Command Line Tools；安装向导管理 IDF 专用 Python、CMake/Ninja/编译器。
- Windows：STEP 2 推荐原生 Windows ESP-IDF；课程 STEP 1 的 WSL Python 不等于
  Windows IDF 工具环境。USB passthrough/权限属于单独路线，不作默认前提。
- Linux：按官方 Get Started 安装系统依赖，确认串口设备用户权限；重新登录使组权限生效。

如果使用官方 CLI 安装路线，以对应 OS 的官方命令为准：clone 指定 v5.5.1，安装 esp32 工具，
在当前终端加载 `export.sh` / `export.ps1` 等环境脚本。不要把自己的课程 `.venv` 当成
ESP-IDF 的 Python 环境；两者目的不同。不要为修证书错误关闭 TLS 校验。

## 3. 确认环境真的可用

在命令面板选择 ESP-IDF 的 Doctor/诊断命令，保存结果。打开 **ESP-IDF Terminal**：

```text
idf.py --version
```

应看到 v5.5.1；如果 `idf.py` 找不到，先检查终端有没有加载 IDF 环境，而不是重装所有软件。
IDF 自己的环境管理工具会检查依赖。扩展菜单能打开，不等于工具链能构建。

## 4. 找到实际串口

插拔前后比较：Windows 设备管理器的 Ports，macOS `/dev/cu.*`，Linux `/dev/ttyUSB*` / `/dev/ttyACM*`。
没有新增设备，先看线、USB hub、桥接芯片驱动与系统枚举，不先修改固件。
仅按实际 USB 桥接芯片与供应商资料安装驱动，别给所有板都装同一个 CH340 驱动。
端口被占用时关闭已有 monitor；不终止未知串口程序。

## 5. 创建 H00 引导工程

在课程根目录，使用已经有的 Python（只是运行工具，不需编写 Python）：

```sh
python3 tools/academy_tutor.py init-project ../guided-desk-tool --guided
```

Windows：

```powershell
py tools/academy_tutor.py init-project ../guided-desk-tool --guided
```

工具仅复制到不存在/空目录，绝不覆盖现有工程。无需课程 `.venv` 依赖。
手工路线：将 `step2/guided-device/` 的全部文件复制到自己的目录，包含隐藏文件；创建
`.academy/course.json`，内容如下（绝对路径替换为本机课程路径，Windows 可用正斜杠）：

```json
{
  "course_root": "/actual/path/esp32-hands-on-academy",
  "manifest": "step2/course.json",
  "start_lesson": "H00"
}
```

在 VS Code 打开引导工程，再将课程目录作为第二个 workspace folder 加入，方便 agent
读取资料。若当前运行权限不允许读取课程，按 IDE 提示添加可读工作区，不绕过权限。
Notebook H00 的启动 prompt 发给 Codex，它会从当前步骤授课。没有 Codex 时自行顺序阅读。

## 6. 构建、烧录和观察

在引导工程 ESP-IDF Terminal 中：

```text
idf.py set-target esp32
idf.py build
idf.py -p <port> flash monitor
```

`<port>` 是占位符，替换为真实 COM5 或 `/dev/cu.usbserial-...` 等。
其他芯片替换 target 并重新核对外设代码。`set-target` 会重新配置构建与 sdkconfig，
已有工程切换 target 前先保存配置。monitor 退出用 **Ctrl+]**，不是只关闭面板。

build 成功证明生成了固件；flash 成功证明写入；启动文本和实物功能分别检查。
H02 创建一次自己的最小工程，H03–H14 在同一工程持续学习。

官方：[扩展安装](https://docs.espressif.com/projects/vscode-esp-idf-extension/en/latest/installation.html)、
[ESP-IDF v5.5.1 Get Started](https://docs.espressif.com/projects/esp-idf/en/v5.5.1/esp32/get-started/index.html)、
[Codex IDE](https://developers.openai.com/codex/ide)。
