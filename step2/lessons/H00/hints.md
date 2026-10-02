# H00 — 分层提示

只读当前步骤的一层，先实际尝试再展开。

## modify — 让固件证明它是新的

<details><summary>Hint 1</summary>

先分别记编辑时间、build 时间和 flash 时间。

</details>

<details><summary>Hint 2</summary>

设备 Reset 只重启 Flash 中的 app，不读取电脑编辑器。

</details>

<details><summary>Hint 3</summary>

先仅 build 后 Reset，记录旧文本；再 flash 对比，形成两份证据。

</details>


## debug — 三个失败属于哪一层

<details><summary>Hint 1</summary>

先找失败发生在 build、flash 还是 monitor。

</details>

<details><summary>Hint 2</summary>

编译发生在电脑；设备只执行已烧录的版本。

</details>

<details><summary>Hint 3</summary>

对比 bin 更新时间与设备启动文本；核对端口枚举。

</details>


## build-own — 写一张设备身份证

<details><summary>Hint 1</summary>

先写别人可以照做的步骤。

</details>

<details><summary>Hint 2</summary>

工具链、板型、串口和应用版本是不同身份。

</details>

<details><summary>Hint 3</summary>

README 按 环境→连接→build→flash→日志→恢复 编排，填写实际值。

</details>
