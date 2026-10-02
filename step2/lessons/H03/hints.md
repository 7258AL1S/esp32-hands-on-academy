# H03 — 分层提示

只读当前步骤的一层，先实际尝试再展开。

## modify — 测量响应而不是猜

<details><summary>Hint 1</summary>

先测开始后的取消响应，不急着改优先级。

</details>

<details><summary>Hint 2</summary>

等待全倒计时会阻塞业务；短 wait 后检查 deadline。

</details>

<details><summary>Hint 3</summary>

保留 started_at/deadline，每轮处理取消再按你定义的优先规则检查到期。

</details>


## debug — 把两秒误写成两毫秒

<details><summary>Hint 1</summary>

先标注所有时间值的单位。

</details>

<details><summary>Hint 2</summary>

raw 边沿、稳定状态和业务事件是不同层。

</details>

<details><summary>Hint 3</summary>

维护 candidate、candidate_since、stable；稳定持续达到阈值才产生状态转换。

</details>


## build-own — 稳定按键 + 可取消计时

<details><summary>Hint 1</summary>

先标注所有时间值的单位。

</details>

<details><summary>Hint 2</summary>

raw 边沿、稳定状态和业务事件是不同层。

</details>

<details><summary>Hint 3</summary>

维护 candidate、candidate_since、stable；稳定持续达到阈值才产生状态转换。

</details>
