# 自动验证

本课不是调用内存函数的假测试。运行：

```sh
.venv/bin/python tools/network_verify.py --lesson 15
```

它会编译 `solution/main.cpp`，让测试客户端或本地 Broker 与 C++ 程序通过真实 `127.0.0.1` socket 通信。`starter` 和 `challenge` 应无法通过对应契约。
