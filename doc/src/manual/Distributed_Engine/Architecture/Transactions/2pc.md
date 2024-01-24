[^_^]:
    两阶段提交
    作者：何国明
    时间：20190817
    评审意见:20190925

本文档将介绍 SequoiaDB 巨杉数据库的二阶段提交机制。

分布式事务
----

在 SequoiaDB 巨杉数据库的的分布式事务中，用户可以通过二阶段提交来保证参与事务的各个数据组之间的 ACID 事务特性。

![pc][pc]

两个阶段是指：

- 准备阶段（Pre-Commit）：在这一阶段，协调节点询问所有参与的数据节点是否准备好提交；参与的数据节点如果已经准备好提交，则回复可以进行提交并进入等待提交状态（Wait-Commit），否则回复不可以进行提交。
- 提交阶段（Commit）：协调者如果在上一阶段收到所有参与的数据节点回复都可以进行提交，则在此阶段向所有参与的数据节点发送提交指令，所有参与的数据节点立即执行提交操作；否则协调者向所有参与的数据节点发送回滚指令，参与的数据节点立即执行回滚操作。

> **Note:**
>
> SequoiaDB 中的数据通常是通过分区集合分布在多个数据复制组的形式进行组织，此处的数据节点是指相应数据复制组的主节点。

事务状态
----

二阶段事务协议把一个事务在某个数据节点的状态进行了划分：

- DOING 状态：事务正在进行中，没有接收到准备提交的命令
- WAIT-COMMIT 状态：数据节点接收到准备提交的命令，并且确认可以提交
- COMMITTED 状态：数据节点接收到提交命令，并且已经提交
- ROLLBACKED 状态：数据节点接收到回滚命令，并且已经回滚

二阶段提交容错方式
----

二阶段提交中的异常主要分为如下三种情况：

- 协调节点正常，参与的数据节点不正常
- 协调节点不正常，参与的数据节点正常
- 协调节点和参与的数据节点都不正常

> **Note:**
>
> 如出现不正常的情况可能由不同的原因引起，如网络不通、节点崩溃或数据复制组切主等。需要确保数据复制组在切主后仍处于相同的事务状态，可以通过 SequoiaDB 的配置参数 transreplsize 控制事务的相关日志写复制组副本的数目来保证。 

对于以上情况，大多数情况下都需要通过参与的数据节点在恢复后进行协商，相互询问事务的状态来确认事务是否应该提交的：

- 如果有一个节点处于 ROLLBACKED 或者 DOING 状态，事务应该回滚。
- 如果有一个节点处于 COMMIT 状态，则事务应该提交。

> **Note:**
>
> - 数据节点的恢复包括重启或数据复制组的主节点切换到同复制组的其他数据节点上
> - 由于两阶段协议的特性，同一个事务中两个参与的数据节点不可能分别处于 DOING，COMMITTED 或者 ROLLBACKED 状态

对于第一种情况：

- 若参与的数据节点在准备阶段不正常，则协调节点收不到准备提交的回复，协调节点不会发送提交命令，而是向其他参与的数据节点发送回滚命令；不正常的数据节点在恢复后通过向其他参与的数据节点协商来进行回滚。
- 若参与的数据节点在提交阶段不正常，当它恢复后可以通过从其它参与的数据节点获取事务是否应该提交，并作出相应的动作。

对于第二种和第三种情况：

- 如果处于准备阶段前不正常，则没有一个参与的数据节点接受到准备提交命令，则所有数据节点在恢复后，都处于 DOING 状态，因此会回滚事务。
- 如果处于准备阶段后不正常，则所有参与事务的节点恢复后都处于 WAIT-COMMIT 状态，则协商后，事务应该提交。
- 如果处于提交或者回滚阶段不正常，则部分参与事务的节点可能已经提交或者回滚，则处于 WAIT-COMMIT 状态的节点在协商后可以作出相应的提交或者回滚的动作。

[^_^]:
    本文使用到的所有链接及引用。

[pc]:images/Distributed_Engine/Architecture/Transactions/pc.png
