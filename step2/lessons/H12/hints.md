# H12 — 分层提示

只读当前步骤的一层，先实际尝试再展开。

## observe — 看见广播不等于配好网

<details><summary>Hint 1</summary>

分别记发现、会话与 IP，不只记“连接成功”。

</details>

<details><summary>Hint 2</summary>

配过网的设备默认不再次广播。

</details>

<details><summary>Hint 3</summary>

查 is_provisioned、手机权限与 app/PoP，再测试已授权的重新配网入口。

</details>


## debug — 有广告却没有授权

<details><summary>Hint 1</summary>

先确认设备是否已 provisioned。

</details>

<details><summary>Hint 2</summary>

BLE 发现与 protocomm 安全会话不同。

</details>

<details><summary>Hint 3</summary>

读取 NVS 配置状态；使用匹配的官方 provisioning 客户端与 PoP，别用跳过认证掩盖问题。

</details>


## build-own — 有边界的配置入口

<details><summary>Hint 1</summary>

先画配网进入/退出/失败状态。

</details>

<details><summary>Hint 2</summary>

凭据来源改变，不代表需要第二套 Wi-Fi 初始化。

</details>

<details><summary>Hint 3</summary>

替换配置入口，复用 H07 owner/重连，设置物理进入方式和配网超时。

</details>
