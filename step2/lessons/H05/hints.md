# H05 — 分层提示

只读当前步骤的一层，先实际尝试再展开。

## modify — 换一种输入节奏

<details><summary>Hint 1</summary>

逐字节记录收到的 chunk。

</details>

<details><summary>Hint 2</summary>

读取块由接收时机决定，不是发送者定义的帧。

</details>

<details><summary>Hint 3</summary>

分别喂一字节、半帧、两帧粘连；比较最终帧应一致。

</details>


## debug — 长度与地址

<details><summary>Hint 1</summary>

记录实际收到的字节数，不先假定消息边界。

</details>

<details><summary>Hint 2</summary>

SPI 长度用 bit；I2C 错地址应返回错误。

</details>

<details><summary>Hint 3</summary>

行 parser 逐字节消费，只有遇到分隔符才提交完整帧；超长帧进入 discard 状态。

</details>


## build-own — 一个有边界的驱动

<details><summary>Hint 1</summary>

记录实际收到的字节数，不先假定消息边界。

</details>

<details><summary>Hint 2</summary>

SPI 长度用 bit；I2C 错地址应返回错误。

</details>

<details><summary>Hint 3</summary>

行 parser 逐字节消费，只有遇到分隔符才提交完整帧；超长帧进入 discard 状态。

</details>
