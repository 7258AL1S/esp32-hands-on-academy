# Guided Desk Tool

这是最小可构建的 ESP-IDF C++ 工程，没有预先完成的课程功能。

在 VS Code 打开本目录，安装 ESP-IDF 扩展，在配置好的 ESP-IDF Terminal 执行：

```text
idf.py --version
idf.py set-target esp32
idf.py build
idf.py -p <port> flash monitor
```

`<port>` 要换成实际串口，例如 COM5 或 /dev/cu.usbserial-xxxx；尖括号不是命令的一部分。
经典 ESP32 使用 target esp32，其他芯片须先核对。monitor 退出用 Ctrl+]。
工具链基线 ESP-IDF v5.5.1；记录实际版本，不把别的版本默认视作已验证。

## 工程地图

- `CMakeLists.txt`：加载 ESP-IDF，定义工程。
- `main/CMakeLists.txt`：登记源码、头文件目录和依赖。
- `main/main.cpp`：C++ 入口，`extern "C"` 保留 app_main 的 C 链接名。
- `sdkconfig.defaults`：可共享配置；`sdkconfig` 是本机生成的配置。
- `build/`：生成文件，不能当作源码编辑。
- `AGENTS.md`：侧边栏学习助理约定，不参与固件编译。
- `.academy/`：课程链接与学习记录，不参与固件运行。

H00–H01 使用此引导工程；H02 再建立自己的最小工程，之后持续扩展。每课前检查 Git diff，按需要保存一次提交；不需要每课建项目。
关闭 Codex 和 Notebook 后，以上官方命令仍可构建、烧录和观察设备。

## 逐步补齐的交接信息

- 实际板型、芯片、工具链与系统：待填写。
- BOM、引脚表、电源要求：待填写。
- 模块/任务/消息设计：待填写。
- build / flash / monitor / 配置步骤：待按本机验证。
- host tests / 网络测试 / 实物验收 / 故障恢复：待填写。
- 软件与硬件许可、第三方依赖：待填写。
