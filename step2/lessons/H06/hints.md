# H06 — 分层提示

只读当前步骤的一层，先实际尝试再展开。

## config — 配置不是散落的常量

<details><summary>Hint 1</summary>

先写一项 int 配置，不同时搬全部参数。

</details>

<details><summary>Hint 2</summary>

menuconfig 生成 CONFIG_ 名；defaults 是共享起点。

</details>

<details><summary>Hint 3</summary>

增加 Kconfig.projbuild、include sdkconfig.h、替换一处 GPIO，并查实际 sdkconfig。

</details>


## debug — 三种错误分层定位

<details><summary>Hint 1</summary>

先把纯函数作为最小拆分对象。

</details>

<details><summary>Hint 2</summary>

公共头使用的类型决定公开依赖。

</details>

<details><summary>Hint 3</summary>

组件 INCLUDE_DIRS 暴露 include；只在 .cpp 用的 Driver 放 PRIV_REQUIRES。

</details>


## host — 复用上层测试

<details><summary>Hint 1</summary>

先挑没有 GPIO/FreeRTOS 依赖的业务状态。

</details>

<details><summary>Hint 2</summary>

同一 Controller 可以由 fake HAL 和真实 Driver 驱动。

</details>

<details><summary>Hint 3</summary>

host 目标只编译 Controller+fake；测试边界和故障，再故意改变条件验证测试会失败。

</details>


## build-own — 整理持续工程

<details><summary>Hint 1</summary>

先把纯函数作为最小拆分对象。

</details>

<details><summary>Hint 2</summary>

公共头使用的类型决定公开依赖。

</details>

<details><summary>Hint 3</summary>

组件 INCLUDE_DIRS 暴露 include；只在 .cpp 用的 Driver 放 PRIV_REQUIRES。

</details>
