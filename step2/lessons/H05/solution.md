# H05 — 尝试后的参考

默认授课路径不读取本文件。明确请求完整参考时再使用；参考实现不记录为独立完成。

地址 Bug 修复为 0x23（ADDR 低）；SPI 四字节 length=4*8。UART 参考状态为 buffer、used、discarding、last_byte_time：正常字节入固定 buffer；满时进入 discard；换行时提交或结束 discard；timeout 清空，保持后续帧可恢复。可以对照 STEP 1 UART 自己通过的 parser。

Guided 示例位于 examples/，完整挑战以行为契约和实际证据验收。开放设计不设唯一答案。

## 可构建的 UART parser 参考

完整参考代码：[line_parser.hpp](solution/line_parser.hpp)。固定 16-byte 容量，换行分帧，
超长帧丢弃到分隔符，1 s 间隔清空半帧；支持显式 expire 检查。callback 需同步消费内容，
不得保存裸指针给异步任务。维护者测试覆盖每字节/半帧/粘帧/溢出恢复/超时/精确容量。
实机把 uart_read_bytes 返回的每个字节送 feed；0 是 timeout，不是空命令。
