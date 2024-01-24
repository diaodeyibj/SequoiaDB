本章将向用户介绍如何安装和部署 SequoiaDB 巨杉数据库。内容主要包括安装前环境准备、安装数据库、数据库部署和集群启停。

环境准备
----

- [软硬件配置要求][env_requirement]：在安装 SequoiaDB 巨杉数据库产品之前，需确认所选操作系统是否支持，且硬件、通信、磁盘和内存是否满足安装要求。
- [Linux 环境配置][linux_suggestion]：在安装 SequoiaDB 巨杉数据库产品之前，建议对操作系统进行相应的配置，以保障系统稳定高效地运行。

规划数据库部署
----

SequoiaDB 采用全分布式的系统架构，支持各种灵活的部署方式。为了更好地发挥硬软件性能，用户需要对系统部署、网络连接等提前做好规划。

SequoiaDB 目前支持两种形式的部署：

- [独立模式][standalone_deployment]

  只在一台物理机上，启动一个数据类型的业务进程。这种模式性能高、安装部署简单方便。缺点是不支持分布式部署，不支持高可用。适用于数据总量不大，总 IOPS 吞吐较小，但对单次操作延时低的场景。

  > **Note:**  
  > 独立模式推荐用于业务的调测开发中，不建议在生产环境使用。

- [集群模式][cluster_deployment]

  可以分布式部署到多物理机上。集群模式需要部署编目节点、数据节点、协调节点以及 Web 管理节点（可选）。每台物理机上可部署任意多个逻辑节点，系统最大支持 65535 个逻辑节点。

在实际生产环境中，用户可根据容量、性能、可靠性、成本等方面的因素，对数据库部署方式进行规划，灵活组合不同的部署方式以满足需求。下述提供几种典型的部署方式供参考。

###最简部署###
最简部署方式适用于对数据量不大，总吞吐不高、可靠性要求不高的应用。

在该部署模式中，SequoiaDB 采用独立模式，只启动一个数据库服务进程。业务应用与数据库既可以部署在同一台服务器上，也可以部署在不同服务器上。

![最简部署][easiest]

###高可用部署###
高可用部署方式适用于对可靠性要求高，但数据量不大、总吞吐要求不高的应用场景。在该部署中，三台物理服务器都部署有数据节点和编目节点，三个数据节点组成一个复制组，三个编目节点组成副本集群。协调节点既可以部署到业务应用所在的服务器上，也可以部署到数据库所在的服务器上。

这种部署方式的优势就是高可靠性。任意一个物理服务器故障，数据的读写都不会受到影响。但缺点是数据容量与单个服务器的容量相同，且硬件成本相对较高。

![高可用部署][available]

SequoiaDB 提供同城双中心、同城三中心、两地三中心和三地五中心的[高可用与容灾部署方案][ha_dr]。

###高性能部署###

高性能部署方式适用于对总数据吞吐或性能要求高、硬件成本低、可靠性要求不高的场景。该部署方式，在三台物理机服务上都部署有编目节点和数据节点，三个编目节点组成一个复制组，每个数据节点分别组成一个单副本的复制组。协调节点与应用部署既可以部署在分离的服务器上，也可以部署在数据库所在服务器上。

这种部署模式可以充分利用所有服务器的存储容量，总的存储容量等于三个服务器的容量总和。但缺点是可靠性较低，任意一台服务器故障，都会导致部分数据无法读取和写入。

![高性能部署][performance]



[^_^]:
    本文使用的所有引用和链接
[env_requirement]:manual/Deployment/env_requirement.md
[linux_suggestion]:manual/Deployment/linux_suggestion.md
[standalone_deployment]:manual/Deployment/standalone_deployment.md
[cluster_deployment]:manual/Deployment/cluster_deployment.md
[easiest]:images/Deployment/easiest.jpg
[available]:images/Deployment/available.jpg
[performance]:images/Deployment/performance.jpg
[ha_dr]:manual/Deployment/ha_dr_program.md