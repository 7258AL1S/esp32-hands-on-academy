# H08 — 分层提示

只读当前步骤的一层，先实际尝试再展开。

## modify — 状态来自 Controller

<details><summary>Hint 1</summary>

API 内容应来自当前 Controller。

</details>

<details><summary>Hint 2</summary>

不同执行上下文读取业务状态需要一致快照。

</details>

<details><summary>Hint 3</summary>

owner 提供只读 snapshot；HTTP 编码这个 snapshot，不直接遍历可变对象。

</details>


## debug — URL、类型与超时

<details><summary>Hint 1</summary>

先看 HTTP status 与 Content-Type，而不是只看正文。

</details>

<details><summary>Hint 2</summary>

content_len 是总长度，recv 是本次块。

</details>

<details><summary>Hint 3</summary>

循环累计已收字节，限制总大小与等待；完整 JSON 校验通过后才提交业务事件。

</details>


## build-own — 安全的局域网控制

<details><summary>Hint 1</summary>

先看 HTTP status 与 Content-Type，而不是只看正文。

</details>

<details><summary>Hint 2</summary>

content_len 是总长度，recv 是本次块。

</details>

<details><summary>Hint 3</summary>

循环累计已收字节，限制总大小与等待；完整 JSON 校验通过后才提交业务事件。

</details>


## udp — 不用记 IP 的发现请求

<details><summary>Hint 1</summary>

先单播，确认设备确实收到了请求。

</details>

<details><summary>Hint 2</summary>

UDP 无连接/到达保证，广播又受 LAN 隔离影响。

</details>

<details><summary>Hint 3</summary>

固定请求版本、限制长度与频率、设置 recv timeout，响应带 ID/port/version。

</details>
