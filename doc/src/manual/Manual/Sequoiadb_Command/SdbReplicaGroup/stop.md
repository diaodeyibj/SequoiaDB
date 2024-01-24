##名称##

stop - 停止当前复制组

##语法##

**rg.stop()**

##类别##

SdbReplicaGroup

##描述##

该函数用于停止当前复制组。

> **Note:**  
>
> 停止后将不能执行创建节点等相关操作，可以通过 [start()][start] 启动节点后操作。

##参数##

无 

##返回值##

函数执行成功时，无返回值。

函数执行失败时，将抛异常并输出错误信息。

##错误##

当异常抛出时，可以通过 [getLastErrMsg()][getLastErrMsg] 获取错误信息或通过 [getLastError()][getLastError] 获取[错误码][error_code]。更多错误处理可以参考[常见错误处理指南][faq]。

##版本##

v2.0 及以上版本

##示例##

停止复制组 group1

```lang-javascript
> var rg = db.getRG("group1")
> rg.stop()
```

[^_^]:
    本文使用的所有引用及链接
[getLastErrMsg]:manual/Manual/Sequoiadb_Command/Global/getLastErrMsg.md
[getLastError]:manual/Manual/Sequoiadb_Command/Global/getLastError.md
[faq]:manual/FAQ/faq_sdb.md
[error_code]:manual/Manual/Sequoiadb_error_code.md
[start]:manual/Manual/Sequoiadb_Command/SdbReplicaGroup/start.md