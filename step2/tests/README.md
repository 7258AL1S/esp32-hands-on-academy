# 验证工具与自己的 C++ 行为

## 维护者参考检查

在课程根目录：

```text
python3 tools/verify_step2.py
.venv/bin/python tools/verify_step2.py --notebooks
.venv/bin/python -m unittest discover -s step2/tests -v
cmake -S step2/tests -B .build/step2-host
cmake --build .build/step2-host
ctest --test-dir .build/step2-host --output-on-failure
```

默认 CMake 测试使用隔离参考实现，**它的通过不代表学生代码通过**。
verify_step2 --firmware 在已加载的 v5.5.1 环境编译 Guided 示例与两个模板，不读取串口或烧录。

## 验证自己的 H03/H05 实现

可以自由设计接口，自己写同等边界测试；也可以使用下列小契约接现有测试。
这些是接口/行为要求，不是完整答案。两个 header 不依赖 ESP-IDF，以便电脑编译。

H03 的 `button_timer.hpp`：

```cpp
namespace desk {
    enum class Event { none, started, cancelled, finished };
    class ButtonTimer {
    public:
        Event update(bool pressed, uint64_t now_ms);
        bool running() const;
    };
}
```

`pressed` 为语义值，`now_ms` 单调增长。稳定 30 ms，长按只产生一次；上电按住不启动，
稳定松开后才允许启动；开始后五秒到期；同轮稳定取消和到期时取消优先。

H05 的 `line_parser.hpp`：

```cpp
namespace desk {
    class LineParser {
    public:
        using Callback = void (*)(const char*, size_t, void*);
        LineParser(Callback callback, void* context);
        void feed(char byte, uint64_t now_ms);
        void expire(uint64_t now_ms);
    };
}
```

固定 16-byte 容量，不包含 delimiter；以 LF 分帧，空帧忽略；超长帧丢弃到 LF；
字节间隔达到 1000 ms 清空半帧。callback 同步消费数据，不保存指针。
若你的 UART 使用 CRLF，增加你自己的归一化层与测试，本测试契约只使用 LF。

构建时**显式指定两个学生 header 的绝对路径**，避免误测 reference：

```text
cmake -S <course-root>/step2/tests -B <my-project>/host-build -DH03_POLICY_HEADER=<absolute-my-header> -DH05_PARSER_HEADER=<absolute-my-header>
cmake --build <my-project>/host-build
ctest --test-dir <my-project>/host-build --output-on-failure
```

路径含空格时在实际终端加引号。这些测试只测软件契约，真实 GPIO/时钟/串口另外验收。
若只完成一课，另一 header 保持维护者参考，报告必须说明只替换了哪个模块，
不能把未替换部分的通过算作自己的完成。

## 真实 HTTP 只读检查

```text
python3 tools/check_device.py --url http://<device-ip>
```

检查提供 URL 的 JSON、增长 uptime 和未知路径 404。可用于 H08 Guided 及自己的 API，
不控制硬件，不更新固件。测试工具的 localhost fixtures 只证明检查器逻辑，不是实机网络证据。
