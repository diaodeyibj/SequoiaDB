[^_^]:
    trace 的原理
    作者：王文净
    时间：20190310
    评审意见
    王涛： 时间：
    许建辉：时间：
    市场部：时间：20190523


trace 是类似 strace 的动态跟踪工具，属于问题诊断和调试工具。

trace 跟踪可以限定到模块、线程、函数和线程类型，相当于屏蔽。通过屏蔽跟踪，可以减少记录的信息量。当进行跟踪时，应尽量使用最小的场景来再现问题，否则被记录的信息量过大时将不利于问题的定位。另外 trace 还提供了在指定函数处打断点进行调试跟踪。断点是调试器的功能之一，可以让线程中断在需要的地方，从而方便分析。

trace 包括收集跟踪日志和解析跟踪日志两部分。

- trace 收集跟踪日志实质上是运行跟踪模块时记录的控制流信息的日志，包括:
  - 调用的函数，调用函数时的时间点和所在源文件的行号，可能还包括调用函数时传入的参数值
  - 函数退出的时间点，可能还包括函数退出时的返回值

   > **Note:**
   > 
   > trace 模块将上述信息组织成二进制格式写入内存中，直到停止 trace 跟踪，才将上述内存中的信息导出为二进制文件。

- trace 解析跟踪日志，是对导出的二进制文件进行格式化及初步分析输出分析文件的过程。它的输出包括：函数调用流程文件、函数的执行时间分析文件、执行时间峰值分析输出文件、记录错误信息的文件和版本文件。
  - 函数调用流程文件：记录跟踪过程中函数之间的调用关系
  - 函数的执行时间分析文件：记录函数调用的次数、平均耗时、最小耗时及最大耗时
  - 执行时间峰值分析输出文件：记录函数调用耗时超过阀值的函数调用信息
  - 记录错误信息的文件：记录解析过程中碰到的不能识别的日志条目
  - 版本文件：记录对应引擎的版本号

