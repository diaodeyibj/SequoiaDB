[^_^]: 

    上下文列表
    作者：何嘉文
    时间：20190522
    评审意见

    王涛：
    许建辉：
    市场部：



上下文列表可以列出所有会话所对应的上下文。

> **Note:**  
> 列表操作会产生一个上下文，因此结果集中至少有当前会话的上下文。

标识
----

SDB_LIST_CONTEXTS

字段信息
----

| 字段名     | 类型       | 描述                    |
| ---------- | ---------- | ----------------------- |
| NodeName   | string     | 上下文所在的节点 |
| SessionID  | int64      | 会话 ID                 |
| TotalCount | int32      | 上下文列表长度          |
| Contexts   | bson array | 上下文 ID 数组，为该会话所包含的所有上下文列表     |

示例
----

查看上下文列表

```lang-javascript
> db.list( SDB_LIST_CONTEXTS )
```

输出结果如下：

```lang-json
{
   "NodeName": "sdbserver1:11830",
   "SessionID": 21,
   "TotalCount": 1,
   "Contexts": [
      182
   ]
}
...
```