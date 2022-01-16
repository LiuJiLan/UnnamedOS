# kernel
- 第一版变量名、函数名尽量与xv6中保持一致

# 笔记

## entry.S

### M态页表
- 根据[rCore教程](https://rcore-os.github.io/rCore-Tutorial-Book-v3/chapter4/3sv39-implementation-1.html)所说, M使用的地址是物理地址还是虚拟地址取决于硬件配置。

### 大页模式
- 大页模式下, offset会变长, 参考[rCore教程](https://rcore-os.github.io/rCore-Tutorial-Book-v3/chapter4/3sv39-implementation-1.html)
- 如果不完整的使用三级页表，未用到的页索引会和虚拟地址的12位默认页内偏移一起形成一个位数更多的**大页**页内偏移
- **注意大页模式下的一些问题**
- 之前由于想完全参考rCore, 所以把内核的虚拟地址设置在0xFFFFFFFFC0200000U
- 这是一个致命的错误:
- 因为我的内核所在的物理地址是0x80000000, 而rCore内核的物理地址是在0x80200000
- (那0x0020_0000用于SBI)
- 我试图在大页模式下将0xFFFFFFFFC0200000U映射到0x80000000, 这是无法实现的
- 一级大页下的效果的本质是把0xFFFFFFFFC映射到了0x000000008
- 后面的部分都是偏移量, 我的错误映射会导致一个0x0020_0000的偏差无法补上
- 由于已经有了二周目补SBI的想法, 我们这里就不做复杂的多级映射去补这个0x0020_0000了
- 所以将memlayout.h中的KERNBASE暂时更改为0xFFFFFFFFC0000000U

### 分页模式的前置条件
- 想开页表先进S-Mode
- (因为M-Mode下是否使用虚拟地址转换是不确定的)
- 如果有PMP系统需要先配置, 不然mret之后会有错误
- x86体系中内核有着最高权限, risc-v中内核上还有M-Mode
- 这也是为什么需要SBI
- 进S-Mode在实际应该在SBI启动的末尾完成, 我们这里只是初期设计, 放入代办事项

### 加载绝对地址
- **risc-v体系下想加载绝对地址是一件麻烦事**
- la伪指令加载的是一个**PC-relative address**
- 但在开页表之后我们需要转跳到虚拟地址, 但这个地址必须以绝对地址的方式提供
- 这里记录一下曾经的尝试:(lx代表la或者li)
- 一开始想完全模仿xv6, 于是使用```lx t0, (V2P_WO(entrypgdir))```
- la报错`offset too large`, li报错`illegal operands`
- 再尝试引入一个新的symbol
- `v2p_entrypgdir = V2P_WO(entrypgdir)`, 然后`la t0, v2p_entrypgdir`
- la报错```elocation truncated to fit: R_RISCV_PCREL_HI20 against symbol `entrypgdir' defined in .data section in main.o```
- li报错`illegal operands`
- **很希望risc-v官方能放开并允许li加载symbol, 可惜暂时不能**
- 所以我们暂时模仿rCore教程使用`lui t0, %hi(entrypgdir)`和`addi t0, t0, %lo(entrypgdir)`
- 但请注意这样加载的其实是32位的, 高位扩展的地址

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

