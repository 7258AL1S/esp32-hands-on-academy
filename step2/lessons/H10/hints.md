# H10 — 分层提示

只读当前步骤的一层，先实际尝试再展开。

## modify — 队列满的行为必须可见

<details><summary>Hint 1</summary>

计算每秒生产与消费数量。

</details>

<details><summary>Hint 2</summary>

任何有限 Queue 都不能长期吸收无限积压。

</details>

<details><summary>Hint 3</summary>

降生产率或选丢弃/覆盖/背压策略，输出 drop 与 sequence 验证。

</details>


## sync — 选择同步工具

<details><summary>Hint 1</summary>

先说明要保护资源，还是等待事件。

</details>

<details><summary>Hint 2</summary>

Mutex 有所有权，Semaphore 表达通知，EventGroup 表达状态。

</details>

<details><summary>Hint 3</summary>

画资源 owner 和等待条件，再选择原语；ISR 只用允许的 FromISR API。

</details>


## debug — 资源顺序造成卡死

<details><summary>Hint 1</summary>

先找谁拥有共享状态。

</details>

<details><summary>Hint 2</summary>

Queue 拷贝字节；锁的等待关系可能成环。

</details>

<details><summary>Hint 3</summary>

用固定结构消息交给单一 owner，统一锁顺序，测消费速率而不是无限扩容。

</details>


## build-own — 证明忙的时候仍能取消

<details><summary>Hint 1</summary>

先找谁拥有共享状态。

</details>

<details><summary>Hint 2</summary>

Queue 拷贝字节；锁的等待关系可能成环。

</details>

<details><summary>Hint 3</summary>

用固定结构消息交给单一 owner，统一锁顺序，测消费速率而不是无限扩容。

</details>
