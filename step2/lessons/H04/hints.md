# H04 — 分层提示

只读当前步骤的一层，先实际尝试再展开。

## modify — 分开采样与显示

<details><summary>Hint 1</summary>

固定旋钮，记录一组 raw，再改滤波。

</details>

<details><summary>Hint 2</summary>

窗口平均降低抖动，也增加响应滞后。

</details>

<details><summary>Hint 3</summary>

用固定数组维护 sum 和实际 count，日志周期独立于采样。

</details>


## calibrate — 把 raw 变成有依据的 mV

<details><summary>Hint 1</summary>

先检查本芯片支持的 calibration scheme。

</details>

<details><summary>Hint 2</summary>

衰减、bit width、efuse 和校准句柄要匹配。

</details>

<details><summary>Hint 3</summary>

创建受支持句柄→raw_to_voltage→对比万用表；不可用则只报告 raw。

</details>


## debug — 忘记提交 duty

<details><summary>Hint 1</summary>

先看 set_duty 后是否调用 update。

</details>

<details><summary>Hint 2</summary>

ADC raw 是量化输出，不是 mV。

</details>

<details><summary>Hint 3</summary>

校准不支持时返回“raw only”；映射前处理范围与错误。

</details>


## build-own — 自动亮度设置

<details><summary>Hint 1</summary>

把读失败和有效零值分开。

</details>

<details><summary>Hint 2</summary>

浮空 raw 不能可靠唯一判定断线；检测需要额外假设。

</details>

<details><summary>Hint 3</summary>

接口返回 value/error，只有有效样本更新固定窗口，定义异常恢复。

</details>
