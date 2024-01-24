[^_^]:
    MySQL 实例-实例管理工具

sdb_mysql_ctl 是 MySQL 实例组件的管理工具。用户通过 sdb_mysql_ctl 既可以初始化、启动和停止实例，也可以修改实例的引擎配置参数。

##参数说明##

| 参数 | 描述 | 是否必填 |
| ---- | ---- | -------- |
| -h | 返回帮助说明 | 否 |
| -D | 指定数据库储存路径 | 是 |
| -l | 指定日志文件，默认在安装路径下，与实例名同名 | 否 |
| -P | 指定 MySQL 服务的监听端口，默认为 3306 | 否 |
| -f | 指定 pid 文件，默认为数据库储存路径下的 `mysqld.pid` | 否 |
| -s | 指定 mysqld.sock 文件，默认为数据库储存路径下的 `mysqld.sock` | 否 |
| -u | 指定连接到 MySQL 服务的数据库用户，默认为 root | 否 |
| -w | 指定连接到 MySQL 服务的密码 | 否 |
| -g | 指定要加入的实例组名，默认为空 | 否 |
| -k | 指定实例组用户密码的密钥，默认为空 | 否 |
| -a | 客户端最大连接数，默认为 1024 | 否 |
| -e | 错误日志级别，默认为 3 | 否 |
| -v | 输出版本信息 | 否 |
| --load-stats | 实例启动过程中加载统计信息 | 否 |
| --sdb-conn-addr | 所连接 SequoiaDB 集群的协调节点地址 | 否 |
| --sdb-user | SequoiaDB 用户名 | 否 |
| --sdb-passwd | SequoiaDB 用户密码 | 否 |
| --sdb-cipherfile | SequoiaDB 用户密码的密文文件路径 | 否 |
| --sdb-token | SequoiaDB 用户密码的加密令牌 | 否 |
| --print | 打印日志信息 | 否 |
| --baklog | 删除实例时是否备份日志文件 | 否 |
               
##使用说明##

运行 sdb_mysql_ctl 工具时，应使用数据库管理用户（安装 MySQL 实例组件时指定，默认为 sdbadmin）权限。

###管理实例###

* 创建实例

    sdb_mysql_ctl addinst \<INSTNAME\> \<-D DATADIR\> [-l LOGFILE] [--print] [-P PORT] [-f PIDFILE] [-s SOCKETFILE] [-w PASSWORD] [-g INST_GROUP_NAME] [-k INST_GROUP_KEY] [--sdb-conn-addr=ADDR] [--sdb-user USER] [--sdb-passwd PASSWD] [--sdb-cipherfile PATH] [--sdb-token TOKEN]
 
    添加一个 myinst 的实例，指定数据库存储路径为 `/opt/sequoiasql/mysql/database/3306/`，指定密码为 123456
 
    ```lang-bash
    $ sdb_mysql_ctl addinst myinst -D /opt/sequoiasql/mysql/database/3306/ -l /opt/sequoiasql/mysql/database/myinst.log --print -P 3306 -f /opt/sequoiasql/mysql/database/myinst.pid -s /opt/sequoiasql/mysql/database/myinst.sock -w 123456 
    ```

* 启动实例

    sdb_mysql_ctl start \<INSTNAME\> [--load-stats [-u=USER] [-w=PASSWD]] [--print]
 
    ```lang-bash
    $ sdb_mysql_ctl start myinst
    ```
    > **Note:**
    >
    > - 指定--load-stats 加载统计信息，可以指定连接 MySQL 服务的用户名和密码；如果不指定，用户名默认为 root，密码采用交互式的方式进行验证。

* 查看实例状态
 
    sdb_mysql_ctl status [INSTNAME]
 
    ```lang-bash 
    $ sdb_mysql_ctl status myinst
    ```
 
* 重启实例
 
    sdb_mysql_ctl restart \<INSTNAME\> [--load-stats [-u=USER] [-w=PASSWD]]

    ```lang-bash
    $ sdb_mysql_ctl restart myinst
    ```

* 停止实例
 
    sdb_mysql_ctl stop \<INSTNAME\> [--print]
 
    ```lang-bash
    $ sdb_mysql_ctl stop myinst 
    ```
 
* 删除实例

    sdb_mysql_ctl delinst \<INSTNAME\> [--baklog]
 
    ```lang-bash
    $ sdb_mysql_ctl delinst myinst
    ```
 
* 查看所有添加的实例

    ```lang-bash
    $ sdb_mysql_ctl listinst
    ```

* 启动所有实例

    ```lang-bash
    $ sdb_mysql_ctl startall [--load-stats [-u=USER] [-w=PASSWD]]
    ```

* 停止所有实例

    ```lang-bash
    $ sdb_mysql_ctl stopall
    ```

* 创建一个实例并加入[实例组][instance_group]

    sdb_mysql_ctl  addinst \<INSTNAME\> \<-D DATADIR\> [-l LOGFILE] [--print] [-P PORT] [-f PIDFILE] [-s SOCKETFILE] [-w PASSWORD] [-g INST_GROUP_NAME] [-k INST_GROUP_KEY] [--sdb-conn-addr=ADDR] [--sdb-user USER] [--sdb-passwd PASSWD] [--sdb-cipherfile PATH] [--sdb-token TOKEN]

    先初始化一个名为“sql_group”实例组

    ```lang-bash
    $ ha_inst_group_init sql_group
    ```

    > **Note：**
    >
    > ha_inst_group_init 的使用可参考[实例组][instance_group]。

    创建一个名为“inst1”的 MySQL 实例并加入 sql_group 实例组

    ```lang-bash
    $ sdb_mysql_ctl addinst inst1  -D /opt/sequoiasql/mysql/database/3306 -P 3306 -g sql_group
    ```

* 已有实例加入实例组

    sdb_mysql_ctl join \<INSTANCE\> \<-g INST_GROUP_NAME\> [-k INST_GROUP_KEY] [--force]
 
    将 inst2 实例加入到 sql_group 实例组

    ```lang-bash
    $ sdb_mysql_ctl join inst2 -g sql_group
    ```

* 从实例组中移除实例

    sdb_mysql_ctl leave \<INSTANCE\> \<-g INST_GROUP_NAME\> [--force]

    从 sql_group 实例组中移除 inst2 实例

    ```lang-bash
    $ sdb_mysql_ctl leave inst2 -g sql_group
    ```

###修改实例的配置###

用户可通过 sdb_mysql_ctl 修改指定实例的 SequoiaDB 引擎配置，各配置项说明可参考 SequoiaDB [引擎配置][config]。实例组功能的使用及相关配置项可参考[实例组][instance_group]。
 
```lang-text
sdb_mysql_ctl chconf <INSTNAME> [-P PORT] [-e LEVEL] [-a MAX-CON]
                     [--sdb-conn-addr=ADDR] [--sdb-user=USER] [--sdb-passwd=PASSWD] [--sdb-auto-partition=BOOL] [--sdb-use-bulk-insert=BOOL]
                     [--sdb-bulk-insert-size=SIZE] [--sdb-use-autocommit=BOOL] 
                     [--sdb-debug-log=BOOL] [--sdb-token=TOKEN] [--sdb-cipherfile=PATH] [--sdb-error-level=ENUM] [--sdb-replica-size=SIZE]
                     [--sdb-use-transaction=BOOL] [--sdb-optimizer-options=SET]
                     [--sdb-rollback-on-timeout=BOOL] [--sdb-execute-only-in-mysql=BOOL]
                     [--sdb-selector-pushdown-threshold=THRESHOLD] [--sdb-alter-table-overhead-threshold=THRESHOLD]
                     [--sdb-lock-wait-timeout=TIMEOUT] [--sdb-use-rollback-segments=BOOL] [--sdb-stats-mode=MODE] [--sdb-stats-sample-num=NUM]
                     [--sdb-stats-sample-percent=DOUBLE] [--sdb-stats-cache=BOOL] [--inst-group-name=NAME] [--inst-group-key=KEY]
``` 

###示例###

修改 myinst 实例的 SequoiaDB 连接地址
 
```lang-bash
$ sdb_mysql_ctl chconf myinst --sdb-conn-addr=sdbserver1:11810,sdbserver2:11810
```

修改过程中需要提供该数据库该实例用户的密码，若未设置 root 用户的密码，在提示密码时直接输入回车即可

```lang-text
Changing configure of instance myinst ...
Enter password:
ok
```


[^_^]:
     本文使用的所有引用和链接
[config]:manual/Database_Instance/Relational_Instance/MySQL_Instance/Maintainance/config.md#SequoiaDB%20引擎配置使用说明
[instance_group]:manual/Database_Instance/Relational_Instance/MySQL_Instance/Installation/instance_group.md