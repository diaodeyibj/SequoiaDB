[^_^]: 

    集合空间列表
    作者：何嘉文
    时间：20190522
    评审意见
    
    王涛：
    许建辉：
    市场部：


集合空间列表：

- 连接非协调节点，列出所有集合空间（不含临时的集合空间）
- 连接协调节点，列出所有集合空间（不含临时的集合空间和系统的集合空间）

标识
----

SDB_LIST_COLLECTIONSPACES

字段信息
----

| 字段名 | 类型   | 描述       |
| ------ | ------ | ---------- |
| Name   | string | 集合空间名 |

示例
---

查看集合空间列表

```lang-javascript
> db.list( SDB_LIST_COLLECTIONSPACES )
```

输出结果如下：

```lang-json
{
  "Name": "sample"
}
...
```