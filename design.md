strict mode in typora `between # and header must be a blank`



TASK_RUNNING 0





系统调用loader, 任何函数都可以嗲用execve函数来调用加载器



Linux程序(不是内存程序)的内存映像



ASLR我们是否设计?

动态库——不实现



ECF (Exceptional Control Flow)

返回地址压入栈中

如果用户态的异常, 压入内核栈

异常处理都在内核态中处理



context 

对象的值: 通用寄存器、浮点寄存器、PC、用户栈、状态寄存器、内核栈

内核数据结构: 页表、进程表、文件表



context switch 

保存当前进程的上下文

恢复接下来要运行的上下文

将控制权交还



程序员视角的进程状态

运行、停止、终止



如何实现fork()调用一次返回两次?



SIGNAL 

Linux Foundation



进程的5种状态

https://www.baeldung.com/linux/process-states



当一个陷阱从特权模式 y 进入特权模式 x 时，xPIE 被设置为 xIE 的值； xIE 设置为 0； xPP 设置为 y。

执行 xRET 指令时，假设 xPP 的值为 y，则 xIE 设置为 xPIE； 权限模式改为y； xPIE 设置为 1； 并且 xPP 设置为最低特权支持模式



全局中断:

**前提: 现在运行于x权级**

xstatus.XIE被设置则x权级全局中断使能; xstatus.XIE被清除则x权级全局中断禁用

对于所有低于x的权级w, 全局中断一定禁用, 无论wIE如何设置

对于所有高于x的权级y, 全局中断一定使能, 无论yIE如何设置



疑问:

RVOS os实验从08开始 不需要在mret之前设置mstatus.MPIE(因为转跳到U态之后M态的全局中断理应使能)
但, 拿os 08-preemptive做实验发现如果没有设置mstatus.MIE系统就不会trap进M态
感觉这点很奇怪

因为还是在M态

# M mode

- mhartid : 放入`tp寄存器`

- mstatus : 主要用于权限转换 & 全局中断

- mtvec : 设置trap进M态的地址

- medeleg & mideleg : 中断的代理

- mip & mie : mie为二级中断

- mscratch : 用于存储M态的栈指针, 注意不能盲目将mscratch直接换给sp, 因为有可能是已经陷入M后发生的二次陷入, 这个时候就需要通过mstatus.MPP判断是否要将mscratch换出
  - 我们无法通过vector模式绕过这个, 因为只有interrupt被单独设置转跳位置, M态处理interrupt中仍可能发生expectations

- mepc : 用于存储异常发生的地址

- mcause : 异常处理的时候用



- pmpaddr0 & pmpcfg0 : 只用这两个PMP寄存器来开启整个内存空间的权限



- CLINT中的msip、mtime & mtimecmp
  - msip : 以内存映射的方式存在, hart间访问
  - mtime & mtimecmp : 此两个寄存器以内存映射的方式存在, 用于定时器中断





栈设计: 对于M和S, 初始化时一个栈, 后续另一个栈

.align 3会使编译器增加 location counter（内存地址）使得可以被8整除

.align 4 被16整除

.balign 4被4整除
