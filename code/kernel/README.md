# kernel
- 第一版变量名、函数名尽量与xv6中保持一致

# 笔记

## entry.S

### M态页表
- 根据[rCore教程](https://rcore-os.github.io/rCore-Tutorial-Book-v3/chapter4/3sv39-implementation-1.html)所说, M使用的地址是物理地址还是虚拟地址取决于硬件配置。

### 大页模式
- 大页模式下, offset会变长, 参考[rCore教程](https://rcore-os.github.io/rCore-Tutorial-Book-v3/chapter4/3sv39-implementation-1.html)
- 如果不完整的使用三级页表，未用到的页索引会和虚拟地址的12位默认页内偏移一起形成一个位数更多的**大页**页内偏移

# 思考

## entry.S

### 权限级问题
- x86体系中, 内核权级正是最高全级, 但是在risc-v中, S态之上还有M态
- 如何设计这里的中断问题是需要思考的一个点

### 进入内核时的状态
- 由于开启分页机制后, M态下的PC是否遵循分页机制是不确定的(后续应该去翻一下文献)
- 所以我们开分页前必须在S态
- rCore的例子中使用了SBI引导启动后处于S态, 我暂时不打算用SBI, 所以要在entry.S中转换成S态
- **第一个版本我想尽量贴合xv6的思路, 之后版本中会自己去写SBI来补足**
