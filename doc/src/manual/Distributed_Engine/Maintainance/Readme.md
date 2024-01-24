[^_^]:
    运维指南
    作者：王涛
    时间：20190213
    评审意见
    王涛：初稿完成；时间：20190213
    许建辉：时间：20190215
    市场部：时间：20190220

SequoiaDB 巨杉数据库运维指南主要涉及数据库的日常操作与维护，包括集群维护、监控、安全、变更、升级、管理、调优、备份、迁移以及问题诊断等内容。

> **Note:**
>
> 使用图形化运维管理界面可参见 [SequoiaDB 管理中心（SAC）][sac]。

数据库管理员在日常维护数据库集群的过程中，主要分为监控、管理和排障三大主题。其中，监控一般作为日常维护操作，通过各种数据库监控指标的组合来说明当前数据库的运行状态。而数据库管理一般包括逻辑物理变更、数据迁移和系统升级等需要对数据库结构、参数或所包含的数据进行直接调整的操作。排障则包括性能调优、故障诊断排查以及故障修复等将数据库恢复正常状态的功能。

通过阅读 SequoiaDB 巨杉数据库运维指南，数据库管理员将会对 SequoiaDB 巨杉数据库的监控、管理及排障等运维体系有综合全面的了解。

[^_^]:
    本文使用到的所有链接及引用。
[sac]:manual/SAC/operating_environment.md