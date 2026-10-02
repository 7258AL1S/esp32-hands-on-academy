# STEP 2 Alpha 验证记录

本版日期：2026-10-02。它是可开始试学的课程 Alpha，不是完成实机验收的硬件套件。

## 当前实际检查

环境：macOS 15.8.1 / Intel x86_64，课程 Python 3.14.7，ESP-IDF **v5.5.1**，
IDF 源码 commit `fcae32885b0296b32044cb99ecbdc50d98dddb83`，target **esp32**，
Xtensa 工具链 `esp-14.2.0_20241119`。IDF 安装在维护者临时工作区，课程不捆绑它。

| 层次 | 结果与范围 |
|---|---|
| 课程结构 | 15 主 Notebook、104 步骤，索引/步骤模式/示例路径/目标文件一致性检查通过 |
| Notebook 执行 | H00–H14 在独立真实 Jupyter kernel 中执行显示格通过；没有执行固件或烧录 |
| 工程保护/读取/证据工具 | 8 项 Python 测试通过：拒绝覆盖、链接/起点、当前步骤隔离、手动证据、HTTP 检查器 |
| host C++ 行为 | 消抖、上电按住、29/30 ms、长按、到期/取消，UART 半帧/粘帧/溢出/超时/容量边界通过 |
| 测试可靠性 | 显式切换另一个 header，30 ms 改为 31 ms 的回归被检测；默认 reference 通过不代表学生实现通过 |
| 固件模板 | device-template 与 guided-device 分别通过真实 ESP-IDF 构建 |
| Guided API 示例 | 21 个 C++ translation unit 使用真实 IDF headers/libraries 联合编译、链接通过 |
| STEP 1 回归 | 按钮控件/C++ 回调/故障/动画/慢放/进程清理测试通过；117 代码格与144源码格式检查通过 |
| VS Code UI | 实际打开并运行 H00，检查暗色文本、复选框、证据输入区；修复黑字和白色 textarea |

Guided 联合构建把示例入口改名以同时编译，不运行它们；它检查 API 类型/源码与链接，
**不证明各课在你的持续工程中的合并已经正确，也不代替独立功能测试**。
HTTP 检查器的 localhost fixture 仅测工具逻辑，不冒充真实设备请求。

## 仍未验收

- 实际板卡接线、供电、电平、LED、按钮、ADC 校准、外设时序。
- ESP32 Wi-Fi/AP、HTTP/WebSocket/MQTT 端到端、BLE 手机配网。
- 真实 FreeRTOS 负载/stack/响应预算、NVS 掉电、Flash OTA 候选/确认/回滚。
- Windows/Linux 硬件路径、其他 ESP32 芯片、其他 IDF/扩展版本。
- 500 px 窄布局与各代码预览在全部 VS Code 配置下的可访问性。
- Codex 作为老师的一整课真实试学（不泄题、不过度提示、环境修复、续学）；
  当前已验证读取/记录工具与规则，不能据此声称模型教学效果通过。

未知项保持未确认，学习者按每课 Hardware Checklist 收集证据。手动填写卡片的绿色
表示“学习者已记录”，不是程序自动测得。不要把历史成绩用于新固件的自动验收。

## 可复现检查

课程根目录，显示检查使用已有课程 .venv，固件检查使用已加载的 ESP-IDF 环境：

```text
python3 tools/verify_step2.py
.venv/bin/python tools/verify_step2.py --notebooks
.venv/bin/python -m unittest discover -s step2/tests -v
cmake -S step2/tests -B .build/step2-host
cmake --build .build/step2-host
ctest --test-dir .build/step2-host --output-on-failure
```

Windows 将 `.venv/bin/python` 换为 `.venv\Scripts\python.exe`，常规工具可使用 py。
上述 Windows 命令形式不等于已完成 Windows 实机验收。

在 ESP-IDF v5.5.1 Terminal：

```text
python tools/verify_step2.py --firmware
```

输出保存 `.build/step2-check/`，工具没有串口参数，不会烧录。更改 API/芯片/配置后重新
构建；源码变化无需把全部 STEP 1 重新跑一遍，共享主题变化回归对应面板。

## 来源和升级

每课列出固定 v5.5.1 官方文档；示例与官方源码/API 核对，网络和 provisioning 参考
同 tag 的 mqtt/tcp、wifi_prov_mgr 例程接口。mDNS 为独立依赖挑战，需学习者选择兼容版
并锁定；该挑战本版未纳入固件示例矩阵，不默认声称构建或实机通过。

升级前记录工具链/组件版本，先构建相关示例，再在自己的板上验证适用路径。
