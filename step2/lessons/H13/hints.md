# H13 — 分层提示

只读当前步骤的一层，先实际尝试再展开。

## modify — 健康不是启动一行日志

<details><summary>Hint 1</summary>

列出产品可以离线工作的部分。

</details>

<details><summary>Hint 2</summary>

拿到 IP 只是 demo 自检，未必是产品健康条件。

</details>

<details><summary>Hint 3</summary>

在 bounded startup 检查关键模块，成功确认、失败回滚，记录 pending/valid。

</details>


## debug — 故意让候选不通过自检

<details><summary>Hint 1</summary>

先检查是否有两个 app 槽与 otadata。

</details>

<details><summary>Hint 2</summary>

pending verify 到 valid 需要你的健康判定。

</details>

<details><summary>Hint 3</summary>

新固件首次启动先自检，失败回滚；在确认前复位能检验 bootloader 回滚路径。

</details>


## build-own — 有边界的更新状态机

<details><summary>Hint 1</summary>

先写用户何时触发更新。

</details>

<details><summary>Hint 2</summary>

下载成功、设置启动槽、健康确认是三个状态转换。

</details>

<details><summary>Hint 3</summary>

完整状态机保留旧版，校验目标/大小/版本，自检后才确认，不重复下载同版。

</details>
