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




