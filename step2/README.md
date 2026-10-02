# STEP 2 — 在同一个工程里学习真实 ESP32

15 个硬件 Notebook 的 Alpha 版本。使用 **VS Code + ESP-IDF + Codex 侧边栏**：
Notebook 保存课程，agent 每次引导一个步骤，源码在正常 ESP-IDF 工程维护。
无需编写 Python，也不需要每课创建工程。没有 Codex 时可顺序读 Notebook 手动完成。

1. 按 [SETUP.md](SETUP.md) 安装扩展/工具链、确认板型与串口。
2. 打开 H00 Notebook，将第一个文本格中的 prompt 发给学习工程 Codex 侧边栏。
3. H00–H01 使用引导工程；H02 亲手创建自己的最小工程。
4. H03–H14 持续扩展同一工程，保留 Git 起点和 `.academy/progress.md`。
5. H14 将同一工程 clone/导出到干净目录，关闭老师和 Notebook，按自己的 README 复现。

[教学机制](TEACHING.md) · [接线与器件](BOARD.md) · [验证证据](VALIDATION.md)

| Lesson | 内容 | Notebook |
|---|---|---|
| H00 | 把第一份固件送进 ESP32 | [H00](notebooks/H00-setup.ipynb) |
| H01 | 把虚拟按钮换成真实 GPIO | [H01](notebooks/H01-gpio.ipynb) |
| H02 | 亲手建立可持续成长的工程 | [H02](notebooks/H02-project.ipynb) |
| H03 | 计时的时候，按钮还要响应 | [H03](notebooks/H03-timing.ipynb) |
| H04 | 让旋钮改变亮度，但别把数字当电压 | [H04](notebooks/H04-measurement.ipynb) |
| H05 | 给工具接上外部设备 | [H05](notebooks/H05-buses.ipynb) |
| H06 | 让工程结构反映职责 | [H06](notebooks/H06-components.ipynb) |
| H07 | 让电脑找到这台工具 | [H07](notebooks/H07-wifi.ipynb) |
| H08 | 浏览器查看真实设备状态 | [H08](notebooks/H08-http.ipynb) |
| H09 | 选择实时推送，而不同时堆所有协议 | [H09](notebooks/H09-realtime.ipynb) |
| H10 | 并发是否真的解决你的问题 | [H10](notebooks/H10-rtos.ipynb) |
| H11 | 重启之后还记得设置 | [H11](notebooks/H11-storage.ipynb) |
| H12 | 没有电脑也能配置网络 | [H12](notebooks/H12-ble.ipynb) |
| H13 | 更新失败，设备还能回来 | [H13](notebooks/H13-ota.ipynb) |
| H14 | 把工具变成自己的项目 | [H14](notebooks/H14-capstone.ipynb) |

## 不隐藏的边界

- C++ 格运行仅预览示例，不自动覆盖工程、构建或烧录；真实操作显示官方命令。
- 代码示例可增量合并，保留现有功能和唯一 app_main。模板没有预先完成的课程功能。
- 主路径不自动读取 Solution；Hint 分层，明确请求完整参考后才读取独立 solution.md。
- Hardware Checklist 绿色表示学习者填写了观察，不表示 Notebook 自动测量实物。
- 本版以 ESP-IDF v5.5.1 / classic ESP32 为基线，其他芯片必须调整并另行验证。
- 构建与 Notebook 验证不能代替实机电气、无线、调度、OTA 或真实授课体验验收。

## 项目结构

```text
step2/
├── notebooks/          H00–H14 主课、启动 prompt、步骤 metadata
├── lessons/Hxx/        Guided 示例、Hint、隔离的参考分析
├── device-template/    无课程功能的最小工程与 AGENTS.md
├── guided-device/      H00–H01 引导工程
├── tests/              工具/教学读取/参考算法的维护者测试
├── course.json         机器可读课程索引
└── SETUP.md            从扩展安装、工具链到串口的手动路径
```

原始教材可人工学习；使用 agent 时不会把它的聊天记忆当作唯一进度记录。
