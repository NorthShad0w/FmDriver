# FM驱动

# 驱动功能
      1.驱动读写 使用 `MmCopyVirtualMemory读取伪造的进程对象作为目标进程 写入使用MmCopyVirtualMemor 失败 则改为Mdl写入
      2.驱动注入 线程注入（伪装隐藏线程）并清空线程回调通知  （ps:如果不禁用 PUBG 会出现内存异常）
      3.获取模块
      4.伪装进程
      5.查询内存
 # 驱动注入演示
https://caiba123.oss-cn-beijing.aliyuncs.com/bandicam%202024-02-17%2018-35-41-357.mp4

# 建议参考
      1.驱动读写 https://github.com/rogxo/ReadPhys
      2.部分函数特征码 NtQueryVirtualMemory NtProtectVirtualMemory 如出现不兼容情况 参考https://github.com/Oxygen1a1/oxgenPdb项目
