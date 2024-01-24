##名称##

fileMD5 - 获取文件的 md5 值

##语法##

***Hash.fileMD5( \<filepath\> )***

##类别##

Hash

##描述##

获取文件的 md5 值。

##参数##

| 参数名   | 参数类型 | 默认值 | 描述     | 是否必填 |
| -------- | -------- | ------ | -------- | -------- |
| filepath | string   | ---    | 文件路径 | 是       |

##返回值##

返回指定文件的 md5 值。

##错误##

如果出错则抛异常，并输出错误信息，可以通过[getLastErrMsg()](manual/Manual/Sequoiadb_Command/Global/getLastErrMsg.md)获取错误信息或通过[getLastError()](manual/Manual/Sequoiadb_Command/Global/getLastError.md)获取错误码。关于错误处理可以参考[常见错误处理指南](manual/FAQ/faq_sdb.md)。

常见错误可参考[错误码](manual/Manual/Sequoiadb_error_code.md)。

##版本##

v3.2 及以上版本

##示例##

获取文件的 md5 值。

```lang-javascript
> Hash.fileMD5( "/opt/sequoiadb/file" )
e2c40f3a729dbe802f2295425d6da21c
```