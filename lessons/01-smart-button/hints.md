# Lesson 01 — 按需查看 Hint

每次只看一层，再回到 Notebook 尝试。

<details>
<summary>Hint 1 — 方向</summary>

当前电平告诉你“现在按着”。怎样知道“刚刚按下”？你需要保存上一次采样的状态。状态要在两次 `tick()` 调用之间保留。

</details>

<details>
<summary>Hint 2 — 关键概念</summary>

本次 `pressed` 为 true、上次 `pressed` 为 false，才是一个新的按下。把上一份状态保存为 `Controller` 的成员变量；不要每次进入 `tick()` 都重新初始化。

</details>

<details>
<summary>Hint 3 — 局部思路</summary>

首次采样只记录 baseline，不反转 LED。之后每次检查新按下，最后无论是否触发，都保存本次输入以供下次使用。你需要把首次采样和后续采样区分开。

</details>
