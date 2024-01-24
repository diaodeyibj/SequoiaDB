[^_^]:
    日志归档
    作者：陈子川
    时间：20190223
    评审意见
    王涛：时间：
    许建辉：时间：
    市场部：时间：20190401


在数据库引擎的多副本机制中，复制组之间的数据同步，主要依赖同步日志完成。同步日志默认在各个节点的 dbpath 路径下的 `replicalog` 目录。同步日志根据节点的[配置文件][node_conf]生成，默认情况下，每个节点的同步日志为 20 个 64MB 的文件。

在数据库引擎运行过程中，同步日志文件将被循环使用，最新产生的日志将会覆盖最老的日志文件。如果用户希望永久保存同步日志，应该选择打开节点的日志归档功能。

通过开启节点的日志归档，用户可以持续归档节点的同步日志，归档的日志不会被覆盖。而且用户可以通过[重放工具][repli_tool]在其它 SequoiaDB 巨杉数据库集群或节点中重新执行。因此用户也可以通过日志归档和重放工具来实现不同集群间的数据同步。

基本功能
----
日志归档包括以下基本功能：

* 归档数据节点的同步日志到本地目录
* 支持压缩存储归档文件
* 归档文件过期自动清理
* 归档目录磁盘配额

开启归档
----
将节点 archiveon 配置参数设为 true 即可开启日志归档功能。首次开启日志归档时，节点会在归档目录下生成用于保证可靠性的归档状态文件 `.archive.1` 和 `.archive.2`，状态文件记录了归档的起始 LSN。

> **Note:**
>
> 开启归档时，节点的 --logfilenum 配置参数必须大于 1。

开启归档后以下两个条件会触发归档操作：

* 当前同步日志文件写满切换到下一个日志文件时
* 归档超时时间段内未发生归档操作

同时，如果节点由于某些原因，例如：节点主降备、异常重启等，导致同步日志 LSN 发生了改变，那么归档文件也会进行相应的修正操作。

归档文件
----

归档文件的文件名有以下几种格式：

| 文件名                   | 说明                                         |
| ------------------------ | -------------------------------------------- |
| archivelog.&lt;FileId>   | 完整归档的日志文件，FileId 是顺序增长的序列号 |
| archivelog.&lt;FileId>.p | 部分归档的日志文件                           |
| archivelog.&lt;FileId>.m | 发生了移动操作的日志文件                     |

> **Note:**
>
> + 在同步日志切换日志文件时，上一个使用的日志文件会被整个归档
> + 如果归档日志开启了压缩功能，节点将会对完整归档的日志文件进行压缩
> + 如果节点执行日志归档的时间超过设置的超时时间，则归档的日志文件仅保存成功归档的部分内容，此时归档的文件称为部分归档文件
> + 当部分归档文件完整归档了同步日志后，会自动转换成完整的归档文件

同步日志归档功能详细配置，可参考节点[配置文件][node_conf]中 archiveon、archivecompresson、archivepath、archivetimeout、archiveexpired 和 archivequota 参数介绍。

错误
----
如果节点发生归档日志失败，会将相关错误信息记录在[节点诊断日志][sdb_diaglog]中，常见错误信息如下表：

| 错误码 | 原因                                                                       | 解决方法                                                         |
| ------ | -------------------------------------------------------------------------- | ---------------------------------------------------------------- |
| -313   | 开启了归档，同步日志的日志文件循环回来，即将要被覆盖的同步日志还没有被归档 | 发生该错误后会触发归档操作，如果连续发生错误需要结合诊断日志分析 |



[^_^]:
    本文使用到的所有链接及引用。
[node_conf]:manual/Distributed_Engine/Maintainance/Database_Configuration/configuration_parameters.md
[repli_tool]:manual/Distributed_Engine/Maintainance/Mgmt_Tools/log_replay.md
[sdb_diaglog]:manual/Distributed_Engine/Maintainance/DiagLog/diaglog.md