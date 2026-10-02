# H07 — 分层提示

只读当前步骤的一层，先实际尝试再展开。

## modify — 切断 AP 再恢复

<details><summary>Hint 1</summary>

先记录断开原因、重试时间和本地响应。

</details>

<details><summary>Hint 2</summary>

服务重连与重新获得 IP 不一定同时完成。

</details>

<details><summary>Hint 3</summary>

用可控 AP 做 down/up，核对 IP 变化和协议重启，不同时改凭据。

</details>


## dns — 名字与地址

<details><summary>Hint 1</summary>

先用已知 IP 建立诊断基线。

</details>

<details><summary>Hint 2</summary>

DNS/mDNS 可失败而 Wi-Fi 仍关联正常。

</details>

<details><summary>Hint 3</summary>

记录 getaddrinfo 返回码，成功后 freeaddrinfo；mDNS 先锁依赖，再核对官方 API。

</details>


## debug — 不要在事件回调睡觉

<details><summary>Hint 1</summary>

先看有没有 IP，再排查名称。

</details>

<details><summary>Hint 2</summary>

关联、DHCP、DNS 和服务监听是不同环节。

</details>

<details><summary>Hint 3</summary>

用已知 IP 访问端口，对照名字访问；退避由 Timer 触发，不在 event handler 等待。

</details>


## build-own — 网络状态接入业务

<details><summary>Hint 1</summary>

先看有没有 IP，再排查名称。

</details>

<details><summary>Hint 2</summary>

关联、DHCP、DNS 和服务监听是不同环节。

</details>

<details><summary>Hint 3</summary>

用已知 IP 访问端口，对照名字访问；退避由 Timer 触发，不在 event handler 等待。

</details>
