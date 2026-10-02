# H02 — 分层提示

只读当前步骤的一层，先实际尝试再展开。

## modify — 把模块换成自己的

<details><summary>Hint 1</summary>

只新增一个函数，先确认它的文件位置。

</details>

<details><summary>Hint 2</summary>

声明、实现、调用和 SRCS 登记缺一可能导致不同错误。

</details>

<details><summary>Hint 3</summary>

从 compile_commands 找实际编译的新文件，再在日志确认新版本。

</details>


## debug — 制造一次链接错误

<details><summary>Hint 1</summary>

确认报错发生在 compiling 还是 linking。

</details>

<details><summary>Hint 2</summary>

header 不等于 source 编译登记。

</details>

<details><summary>Hint 3</summary>

检查 idf_component_register(SRCS ...) 是否列出实现文件，再核对函数签名。

</details>


## build-own — 把按钮行为带进来

<details><summary>Hint 1</summary>

确认报错发生在 compiling 还是 linking。

</details>

<details><summary>Hint 2</summary>

header 不等于 source 编译登记。

</details>

<details><summary>Hint 3</summary>

检查 idf_component_register(SRCS ...) 是否列出实现文件，再核对函数签名。

</details>
