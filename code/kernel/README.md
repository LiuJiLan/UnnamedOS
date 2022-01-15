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

## The So F--king **li** and **la**
- 我是图通过```la t0, entrypgdir```去把页表加进去, 然后减去虚拟地址和实际地址的差值
- 然后我发现有错误, 原因是上述指令会把页表的实地址加载过去
- 因为加载的是一个**PC-relative address**, 而代码实际被加载的位置是物理地址
- 所以即使链接脚本用的是**虚拟地址**也会导致加载进去的是**物理地址**
- 那我们如何加载想要的虚拟地址呢, 说白了加载**绝对地址**就好了
- 那我们能用li加载立即数的方式加载绝对地址吗?
- 不能!!! 因为这样会引起```Error: illegal operands```
- 自此我终于能理解rCore中为什么要以那么诡异的方式加载页表了
- [以下代码出自rCore教程](https://rcore-os.github.io/rCore-Tutorial-deploy/docs/lab-3/guide/part-2.html)
```
_start:
    # 计算 boot_page_table 的物理页号
    lui t0, %hi(boot_page_table)
    li t1, 0xffffffff00000000
    sub t0, t0, t1
    srli t0, t0, 12
    # 8 << 60 是 satp 中使用 Sv39 模式的记号
    li t1, (8 << 60)
    or t0, t0, t1
    # 写入 satp 并更新 TLB
    csrw satp, t0
    sfence.vma

    # 加载栈地址
    lui sp, %hi(boot_stack_top)
    addi sp, sp, %lo(boot_stack_top)
    # 跳转至 rust_main
    lui t0, %hi(rust_main)
    addi t0, t0, %lo(rust_main)
    jr t0
```
- 因为不能直接用伪指令li加载一个symbol, 所以我们只能用%hi和%lo加载两次
- (尽管这是一条伪指令, 最后一定会被展开成两条指令)
- **但既然我们能把symbol当作一个立即数, 为什么我们不能用li加载呢?!?**
