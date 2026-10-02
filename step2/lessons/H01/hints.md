# H01 — 分层提示

只读当前步骤的一层，先实际尝试再展开。

## modify — 更换观察方法

<details><summary>Hint 1</summary>

对比输出日志与实际 LED。

</details>

<details><summary>Hint 2</summary>

GPIO 寄存器状态不能检测断开的 LED 支路。

</details>

<details><summary>Hint 3</summary>

记录同一时间 output 和灯光；改线先断电。

</details>


## debug — 按钮反了还是浮空了

<details><summary>Hint 1</summary>

先分开记录输入 raw 与输出状态。

</details>

<details><summary>Hint 2</summary>

上拉决定松开状态；pressed 是业务语义。

</details>

<details><summary>Hint 3</summary>

GPIO 初始化与 LOW/HIGH 换算留在 Esp32Board，Controller 只使用契约。

</details>


## build-own — 迁移软件契约

<details><summary>Hint 1</summary>

先列出 Controller 真正需要的两个接口。

</details>

<details><summary>Hint 2</summary>

软件接口保持 raw LOW=pressed 契约；硬件实现处理引脚与初始化。

</details>

<details><summary>Hint 3</summary>

注入 Board 引用，用 fake 测业务边沿；实机再测极性。

</details>
