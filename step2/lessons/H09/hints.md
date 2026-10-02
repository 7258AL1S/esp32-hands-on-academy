# H09 — 分层提示

只读当前步骤的一层，先实际尝试再展开。

## modify — 状态与事件分开

<details><summary>Hint 1</summary>

让新订阅者加入，再比较两种消息。

</details>

<details><summary>Hint 2</summary>

retain 重放最后状态，事件没有这种语义。

</details>

<details><summary>Hint 3</summary>

state retain=true，event retain=false；重启 Broker 验证订阅恢复。

</details>


## debug — 旧状态不是新动作

<details><summary>Hint 1</summary>

先区分状态和一次性动作。

</details>

<details><summary>Hint 2</summary>

retain 保存最后消息，QoS 1 允许重复。

</details>

<details><summary>Hint 3</summary>

只在 offset+data_len 达到 total 时尝试解析；容量/偏移/重复 ID 必须验证。

</details>


## build-own — 完成一种实时通道

<details><summary>Hint 1</summary>

先区分状态和一次性动作。

</details>

<details><summary>Hint 2</summary>

retain 保存最后消息，QoS 1 允许重复。

</details>

<details><summary>Hint 3</summary>

只在 offset+data_len 达到 total 时尝试解析；容量/偏移/重复 ID 必须验证。

</details>
