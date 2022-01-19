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

## main.c

### 引入外部symbol
- `extern char end[];`其实是链接脚本提供的外部symbol
- 它标示着内核的结束位置
- 我尝试过声明为`extern void * end;`和`extern char * end;`
- 但是这都会导致`end`不能被正常引入, 之后应该去看看资料
- 但照着xv6这样引入反正不会错

### kinit1(end, (void *)P2V(0x80000000 + 0x200000));
- 我们暂时初始化2MB的空间, xv6只初始化了4MB
- 注意xv6当中是直接给的4MB的值, 因为认为内存起点在0处
- 给出4MB的值的原因是, x86大页模式下是4MB的大页
- 我们用的Sv39其实能给出1GiB的大页, 其实我们一开始就能初始化完全部
- 我们这里只初始化2MB是为了流一部分空间给给kinit2去实验初始化
- (如果按通常的设计2MB给SBI, kinit1()给2MB, kinit2()再给2MB)
- (刚好用完k210的6MB, 尽管有方法可以用到8MB, 但是暂时不去管)

## kalloc.c

### 如果暂时没法完成
- 在kalloc中我们使用了spinlock, 但是我们暂时还没有写到哪里
- 事实上在main.c初次用到的kinit1()尽管初始化了lock, 但却没有使用它
- 我们可以暂时注释掉
- 我们还用到了console中的panic()函数
- 不得不说printf("Hello World\n");在写内核的时候是一件大难事
- panic()也是类似, 所以我只是在panic()中做了一个原地tp
- 暂时`char * target = s;`, 这样方便去读s的地址然后在gdb查看它
- 但其实都不需要原地tp, 因为gdb会提供信息, 例如:
- `Breakpoint 2, panic (s=0xffffffffc0000548 "GOOD!") at console.c:11`

### kfree()里指针的快乐
- 很多人评价说C语言最痛苦就是指针, 我不这样认为
- kfree()中`r = (struct run*)v;`将页里的一部分部分直接用来存储指针
- 这个设计真的令人感到精妙无比

## vm.c

### 如果暂时没法完成
- 只完成kvmalloc的部分
- setupkvm()函数中会有freevm(), xv6中在注释中说实在用户部分用的
- 暂不实现, 暂用`panic("freevm");`代替

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

# To Do List
- 区别于最顶层README的to-do-list, 这里记录一些局部小问题

## To Do
* [X] mret问题                                entry.S
* [ ] NULL的问题, 还有部分函数要改得更精细一些     string.c
* [ ] 去查查有关"引入外部symbol"的资料           main.c
* [ ] 关于`pde_t`命名的问题                    main.c vm.c mmu.h type.h
* [ ] 关于`PTX`命名的问题                      mmu.h type.h
* [ ] `ptrdiff_t`还是`intptr_t`               vm.c type.h

## Done
