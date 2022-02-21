# SBI

# 笔记

## 中断处理

### 不能用jr转跳
- 如果全是向ecall这样的主动中断, 是可以的, 因为可以在发生前保存寄存器
- 但除此之外的中断会直接跳到中断处理的symbol处, 如果用jr就会破坏寄存器
- 所以同时保存寄存器的工作也只能在这里做

### mcause和mtval
- mtval只放置exception的额外信息
- 如果我们mtvec采取vectored我们可以不用在interrupt中传入mcause和mtval
- (因为mcause的内容已经被自动设置pc转跳, 而mtval只放置exception的信息)

### sp指针
- 如果从开了页表后的S态trap到M态, sp指针会指向虚拟地址
- 但M态不使用页表, 所以会导致M态访问错误
- 如果使用原sp去转换成物理地址, 不考虑临界区大致可以用以下思路:
```
csrw    mscratch, t0    //暂存t0
li      t0, 虚拟地址和物理地址差值
sub     sp, sp, t0
csrr    t0, mscratch    //恢复t0
```
- 但这样做有一个问题, 我们要判断是否由M态trap, 如果是M态trap则不需要减去差值
- (还有可能我们trap的时候虽在S态却没有开页表)
- 而且我没有去仔细考虑这会不会影响到多核下对临界区的处理
- 我们参考一下[openSBI](https://github.com/riscv-software-src/opensbi/blob/master/firmware/fw_base.S) 中`TRAP_SAVE_AND_SETUP_SP_T0`
- 我暂设的逻辑是:
```
swap(mscratch, t0)          //  mscratch是每个hart的栈的栈顶, csrrw t0, mscratch, t0
save_who_to_stack(t1, t0)   //  第一个是对象, 第二个是栈顶, t1入栈
save_who_to_stack(t2, t0)   //  存原t2
t1 = t0 - 临时空间大小        //  临时空间就是hart的栈的栈顶之下的一小部分, 用于存一些临时变量
                            //  如果不是从M态trap, 则栈顶在临时空间大小之后
t2 = sp                     //  t2 = 原sp
sp = if (MSTATUS.MPP != M-Mode) return t1; else return t2;
save_who_to_stack(t2, sp)   // 保存之前的原sp
t1 = 原t1                    
t2 = 原t2                   //  hart的栈的栈顶还在由t0维护, 所以先恢复这两个
swap(mscratch, t0)          // 将原t0换回, mscratch继续保存hart的栈的栈顶
//  要将mscratch换出的本质是因为它是一个CSR寄存器, 不能直接对他使用sd/ld指令去控制栈
//  设置sp具体的方法是:
set()   sp, MSTATUS.MPP //  伪代码, 实际上csrr + srl来实现
andi    sp, sp, 0b11    //  0b11是M-Mode编号, 与上后如果不是M-Mode必然比0b11小
slti    sp, sp, 0b11    //  if (M-Mode) sp = 0; else sp = 1;
addi    sp, sp, -1      //  sp -= 1 (制造0b1...1和0b0...0)
xor     t2, t2, t1
and     sp, sp, t2
xor     t2, t2, t1
xor     sp, t1, sp      //  神奇的位运算, 结束后能保持t1和t2不变
```

## sbi_trap
- 参考了openSBI

## IPI与CLINT

### Legacy Extensions与IPI Extension之别
- Legacy Extensions传递的是hart_mask的指针
- 而IPI Extension则改为传值
- 因为M态不使用页表, 可能会产生地址访问错误
- 但是我暂时不想实现其他的SBI Extensions
- 在此处规定, Legacy Extensions传递的一律使用物理地址传值
- 后续改进中将kernel内核中的sbi_send_ipi再改为IPI Extension
- 
- 但要注意, 根据手册:
- `MSIP is read-only in mip`
- ```and is written by accesses to memory-mapped control registers```
- 
- 但是, 对于sie和sip:
- ```The sip and sie registers are subsets of the mip and mie registers. Reading any implemented field, or writing any writable field, of sip/sie effects a read or write of the homonymous field of mip/mie.```

### 通过内存映射访问CLINT
- 注意在访问的时候需要选择正确的数据长度
- 否则会出现Store/AMO access fault
- 例如不能声明为unsigned char, 要按照手册的4Bytes来
- (尽管本质上只用向内存写入一个bit)

### wfi与唤醒
- 根据手册, wfi不受部分中断设置的影响
- 具体而言:
- 1、即使mstatus中的mie位的设为屏蔽中断, wfi仍会被中断唤醒
- 2、在mie中屏蔽过的中断不会唤醒wfi状态的hart

### 触发sip.SSIP
- 需要注意的点与sbi_set_timer()中的触发sip.STIP的注意事项相同

### 不在需要sbi_clear_ipi()
- 手册的解释是
- ```sbi_clear_ipi() is deprecated because S-mode code can clear sip.SSIP CSR bit directly.```
- 大概率推测, mip.MSIP应该是由M态的trap给处理清楚的
- sip.SSIP在S态是可写的

## sbi_set_timer与代理机制

### 有关触发sip.STIP
- **可能理解有误:**
- 如果只去设置mtimecmp只会引发M态中断,
- 我们尝试能有硬件的方法直接在mtime >= mtimecmp的时候引发STIP
- 我们考虑的第一个想法是使用代理:
- FU540的手册很容易让人误解, 8.4节中对应寄存器的例子
- Field Name是MSIP, 作用是Delegate Supervisor Software Interrupt
- 可能会让人以为是将MSIP位代理给SSIP位
- (相当于MSIP的pending会反映在SSIP位上)
- **但是就其他资料而言这是个错误的理解:**
- `m*deleg`寄存器代理的行为因该是被代理的trap会被导向S态的一套寄存器
- 例如陷入的位置是stvec而不是mtvec
- **但这就会引发一个新的问题:**
- 如果观察sip, S态其实读不到pending
- (因为除了S态的interrupt其他位都是硬置0)(**其实并不是**)
- 在这样的情况下, 我们只能通过M态转发
- 详细而言是, 我们设置mtimecmp之后, 还要设置M态的trap
- 让其在遇到MTIP时, 软件的设置STIP位, 从而达到触发STIP的效果
- (`m*deleg`还是需要按需设置, 不然S态的中断也要扔回M态处理)
- 当发现这一点时会觉得RISC-V设计的如此不合理, 
- 通知S态发生了定时器中断竟需要如此多的软件步骤
- **但是:**
- 特权手册有关sie与sip部分:
- ```Bits 3, 7, and 11 of sip and sie correspond to the machine-mode software, timer, and external interrupts, respectively. Since most platforms will choose not to make these interrupts delegatable from M-mode to S-mode, they are shown as 0 in Figures 4.6 and 4.7.```
- 这说明在硬件设计上其实没有规定死S态不能代理M态中断
- (本身有计划去复现一个可以支持我们这个系统的硬件平台, 所以我会关注这些)
- 但是新问题又来了, scause中有关M态中断的位是置零的
- 不知道在此处ISA会不会也有相应的放宽处理

### 实现与设计
- 可能由于上面的原因, openSBI和rustSBI都采取了转发的方式
- (这或许也是模拟FU540物理平台的qemu的唯一途径)
- 应该注意的是:
- sbi_set_timer()部分只去关心定时器的设置, 
- 有关转发的工作应该由M态(也就是SBI的)trap去关心
- (有部分资料中把这些部分称为runtime)

### 有关清除sip.STIP
- 因为:
- ```If implemented, STIP is read-only in sip, and is set and cleared by the execution environment.```
- 所以相应的清除应该由M态来做

### 有关清除mip.MTIP
- 重设mtimecmp后, 如果mtimecmp > mtime的话, MTIP自然会清除
- 但是, 我们还是需要在M态trap中清除MTIP
- 我们可以直接采用给予一个interval使mtimecmp重新大于mtime的方法
- 但是如果考虑到如果S态本身不希望有计时器中断或不希望再有计时器中断
- 而M态还一直在维护一个仅为了清除MTIP而存在的计时器
- 这是非常不合理的(处理中断本身就是一种消耗)
- 我们采取的方案是直接设置设置一个infinite的mtimecmp
- (也就是-1, 0b11...11)
- 注意这个方案与`sbi_set_timer()`手册中的不同:
- ```it can either request a timer interrupt infinitely far into the future (i.e., (uint64_t)-1)```
- 因为手册中的意义是给出-1的interval就相当于设置了最长时间
- 而实际上interval会加上mtime去设置mtimecmp(会溢出而达不到效果)
- 我没有在`sbi_set_timer()`实现这个功能, 这个功能的实现参考手册:
- ```or it can instead mask the timer interrupt by clearing sie.STIE CSR bit.```
- **我直接设置mtimecmp只是为了在M态trap中清除MTIP而已!**

