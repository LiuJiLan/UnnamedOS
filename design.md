# 设计

---

**注意: 本文件并非全部纯手工书写, 部分样式直接由MarkDown书写软件自动转化。**

**固: 直接阅读Raw Code会略显混乱, 请使用相应软件或网站查看本文档。**





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



fork()一次调用与两次返回

​	fork的作用是在陷入kernel后开辟一个完全一样的task / process副本

​	对于父进程而言, 好似什么都没有发生, 但对于子进程而言也是一样(就像他的父进程刚刚获取了fork的返回值一样)

​	由于这个时候父子进程同时被扔入进程调度中, 所以实际上谁会被先运行(假设是单线程)是与进程调度机制相关的



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



栈设计: 对于M和S, 初始化时一个栈, 后续另一个栈

.align 3会使编译器增加 location counter（内存地址）使得可以被8整除

.align 4 被16整除

.balign 4被4整除



.rept & .endr



错误代码

https://baike.baidu.com/item/errno/11040395?fr=aladdin



sleep是不精准的, 只是再预定时间后重新回到调用队列



有关clone与SYS_clone

```c
pid_t clone(int (*fn)(void *arg), void *arg, void *stack, size_t stack_size, unsigned long flags)
{
    if (stack)
	stack += stack_size;

    return __clone(fn, stack, flags, NULL, NULL, NULL);
    //return syscall(SYS_clone, fn, stack, flags, NULL, NULL, NULL);
}
```



```assembly
# __clone(func, stack, flags, arg, ptid, tls, ctid)
#           a0,    a1,    a2,  a3,   a4,  a5,   a6

# syscall(SYS_clone, flags, stack, ptid, tls, ctid)
#                a7     a0,    a1,   a2,  a3,   a4

.global __clone
.type  __clone, %function
__clone:
	# Save func and arg to stack
	addi a1, a1, -16
	sd a0, 0(a1)
	sd a3, 8(a1)

	# Call SYS_clone
	mv a0, a2
	mv a2, a4
	mv a3, a5
	mv a4, a6
	li a7, 220 # SYS_clone
	ecall

	beqz a0, 1f
	# Parent
	ret

	# Child
1:      ld a1, 0(sp)
	ld a0, 8(sp)
	jalr a1

	# Exit
	li a7, 93 # SYS_exit
	ecall
```

```c
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

size_t stack[1024] = {0};
static int child_pid;

static int child_func(void){
    printf("  Child says successfully!\n");
    return 0;
}

void test_clone(void){
    TEST_START(__func__);
    int wstatus;
    child_pid = clone(child_func, NULL, stack, 1024, SIGCHLD);
    assert(child_pid != -1);
    if (child_pid == 0){
	exit(0);
    }else{
	if(wait(&wstatus) == child_pid)
	    printf("clone process successfully.\npid:%d\n", child_pid);
	else
	    printf("clone process error.\n");
    }

    TEST_END(__func__);
}

int main(void){
    test_clone();
    return 0;
}
```





/Library/Developer/CommandLineTools/SDKs/MacOSX12.3.sdk/System/Library/Frameworks/Kernel.framework/Versions/A/Headers/sys



句柄与文件描述符, https://datacadamia.com/file/file_descriptor, 都是用于对应文件的标识, 不知道为什么组委会会在linux的文档里使用句柄一词

自旋锁OK嘛?

一个有关自旋锁的问题, 
xv6-riscv的进程调度是每次遍历进程表, 然后找到RUNNABLE的进程
然后每个hart遍历进程表的时候,都会给表上锁
那会不会出现渴望进程变成RUNNABLE的hart(或hart们)一直给表上锁, 导致能给表带来新RUNNABLE的hart拥有不了表



解决方法, 就是如果一个CPU无进程可运行的情况不带锁的去查表, 如果发现有可运行再去请求锁, 再遍历, 因为寻找有没有RUNNABLE的过程本质是只读的, 只有涉及读写的时候才上锁

或者本身表就不上锁, 只上表项的锁, 因为自旋本身就能保证竞争, 只要在竞争到资源后再次确认就好了





# VM

设计的时候很纠结, 因为可能会涉及到会不会犯错

> 1. 由于映射一对虚实地址时, 会知道子级页表是否存在, 不存在则新建(此时会alloc空间); 但取消映射时不清楚一张子级表中的表项(entry)是否全部废除(需要遍历), 处于效率考虑则不做这个遍历, 但又应该适时清楚完全没有用的子级页表
> 2. 递归删除(完全删除)一个主表表项(相当于完全回收2GiB空间内被分配的空间)是考虑提供的API, 但我知道后续设计的时候, 我会考虑对于每个进程的用户页表填充对内核页表的引用(与fork进程的效果不同, 用户页表中有表项指向同样的内核物理空间), 但是明显释放用户页表时不能将这些内核表项递归删除

所以采取的方法是设计两层API

第一层假设程序员不会犯错, 会小心谨慎的使用给出的API, 完全相信程序员

第二层根据相关用法提供一些API(例如在回收一个用户页表时清空所有的用于低地址空间及之前alloc的子级页表)

inverse() = $map^{-1}$()

```python
map(pgtbl, va, pa, perm)	#	映射; 如果子级页表未创建自动创建
inverse(pgtbl, va)->pa		# return pa according to pgtbl(函数名参考反函数一词)
unmap(pgtbl, va)					# 解除映射; 不考虑被解除的页表项的页表是否已经完全无用
free(pgtbl)								#	删除一个页表/子级页表 直接用kfree就好

对于kpgtbl


```





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



# S mode

- sstatus : mstatus的子集
- stvec : S态中断
- sip & sie : 控制中断
- sscratch : 用于调节中断处理
- sepc : 提供返回地址, 本质上保存了不可见的pc寄存器
- scause : trap的原因
- satp : 页表



---



# 进程管理

## 同步

race condition

P179 单核处理器, 修改共享变量时只要禁止中断



## 进程的状态

## 参考资料

https://www.geek-share.com/detail/2711546635.html

```c
#define TASK_RUNNING    0 
#define TASK_INTERRUPTIBLE  1 
#define TASK_UNINTERRUPTIBLE  2 
#define __TASK_STOPPED    4 
#define __TASK_TRACED    8 
/* in tsk->exit_state */ 
#define EXIT_ZOMBIE    16 
#define EXIT_DEAD    32 
/* in tsk->state again */ 
#define TASK_DEAD    64 
#define TASK_WAKEKILL    128 
#define TASK_WAKING    256 
#define TASK_STATE_MAX    512 
```

注意使用的是bit-maps / bit-mask

```c
static const char * const task_state_array[] = {
"R (running)", /* 0 */
"S (sleeping)", /* 1 */
"D (disk sleep)", /* 2 */
"T (stopped)", /* 4 */
"t (tracing stop)", /* 8 */
"X (dead)", /* 16 */
"Z (zombie)", /* 32 */
};
```



## 我的设计



| 实际状态                          |      state      |     sched     |  usable  |
| :-------------------------------- | :-------------: | :-----------: | :------: |
| CPU上运行 (RUNNING的一种表现形式) |     RUNNING     | UNSCHEDULABLE | UNUSABLE |
| RUNNABLE (RUNNING的一种表现形式)  |     RUNNING     |  SCHEDULABLE  | UNUSABLE |
| INTERRUPTIBLE (本质是sleep)       |  INTERRUPTIBLE  | UNSCHEDULABLE | UNUSABLE |
| ZOMBIE                            |     ZOMBIE      | UNSCHEDULABLE | UNUSABLE |
| DEAD (相当于释放了)               | NULL (填ZOMBIE) | UNSCHEDULABLE |  USABLE  |



| 内部命名 | 状态转移起点 | 状态转移终点 | 实际作用                               | 对应系统调用部分 |
| -------- | ------------ | ------------ | -------------------------------------- | ---------------- |
|          |              |              | 在进程列表中挑选一个                   |                  |
| fork     | CPU上运行    | RUNNABLE     | 复制一个进程到进程队列, 并标为RUNNABLE |                  |
| wait     | ZOMBIE       | DEAD         |                                        |                  |
|          |              |              |                                        |                  |



### SYS_clone

```
#	锁: 需要获取多个进程项

flags, stack, ptid, tls, ctid
除stack之外忽略
注意:用户程序中传入的stack是栈低
而系统调用中的stack是栈顶, 中间已经有过转化

找到一个usable进程
复制本进程
设置父进程等参数
设置栈
if stack == 0:
	sp不变 # 保留父进程的状态
else 
	sp = stack

成功返回子进程ID
失败返回-1

对子进程返回0

#	详细代码

找到一个usable进程
if	找不到合适的usable进程:
	直接返回-1
复制本进程
if 	复制进程失败
	直接返回-1
```



### SYS_exit

```
#	锁: 需要获取多个进程项 (因为要对父进程进行写操作)

设自己的xstate为返回值
将自己的所有子进程的ppid改为自己的父进程 (重要步骤)
将自己所有的子进程加入父进程的cpid_bitmap (由于我们的设计, 只需要or就可以了)
将自己所有的等待集合加入父进程的zombie子进程集合 (由于我们的设计, 只需要or就可以了)
设置自己的一些值确保进入ZOMBIE
如果父进程 处于sleep 且 sleep的原因是SYS_wait4 (通过查看寄存器得知)
设置父进程为RUNNABLE
```



### SYS_wait4

```
#	锁: 需要获取多个进程项 (因为要对父进程进行写操作)

pid_t pid, int *wstatus, int options, struct rusage *rusage

*wstatus 存放对应子进程的返回值, 可以给NULL; 注意不支持对应的分析功能, 假定一定是通过exit或许return退出的
options支持:
0, WNOHANG, WUNTRACED (WCONTINUED暂不支持)

if 没有子进程:
	直接返回-1
	
if 没有等待集合满足要求:
	if WNOHANG:
		立即返回0
	else 
		自己进入sleep
	
if 满足要求:
	# 无论是否是多个满足要求, 只返回满足集合中的第一个(pid最小)
	
	if (wstatus != NULL):
		*wstatus = c_proc.xstate
	cpid = c_proc.pid
	if	WUNTRACED:
		return cpid
	else:
		将cpid的那个进程从ZOMBIE变为DEAD
		return cpid
```

### SYS_shed_yield

```
注意, 默认一定成功, 成功返回0
```





# 储存管理

## I/O系统

read调用与阻塞or非阻塞是无关的, read只负责去读写, 而设备的性质才去决定 



## 文件

https://blog.csdn.net/weixin_47397155/article/details/123017512

文件的偏移量

https://www.csdn.net/tags/MtzaAg0sODU5ODgtYmxvZwO0O0OO0O0O.html

https://blog.csdn.net/m0_57908316/article/details/120815061

偏移量超出



## execve会打开文件吗?

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, const char * argv[]) {
    pid_t par = getpid();
    printf("Parent PID is %d\n", par);
    
    pid_t chi = fork();
    if (chi == 0) { // child
        printf("I AM CHILD\n");
        char * new_e[] = {"test", NULL};
        char * new_p[] = {NULL};
        execve("/Users/liujilan/Desktop/LDC/Competition/2022_OS/test_11_execve_and_file/test_11_execve_and_file/test.out", new_e, new_p);
    } else {
        printf("Child PID is %d\n", chi);
        waitpid(-1, NULL, 0);
    }
    
    return 0;
}
```

```c
#include <stdio.h>

int main(int argc, const char * argv[]) {
    printf("CHILD IS ON\n");
    char ch = 0;
    while (ch != 'a') {
        ch = getchar();
        printf("%c", ch);
    }
    
    return 0;
}
```

```shell
Parent PID is 91054
Child PID is 91057
I AM CHILD
CHILD IS ON
a
aProgram ended with exit code: 0
```

```shell
liujilan@LiuJiLans-MBP-2019 ~ % lsof -p 91054
COMMAND     PID     USER   FD   TYPE DEVICE  SIZE/OFF                NODE NAME
test_11_e 91054 liujilan  cwd    DIR    1,4        96           147211462 /Users/liujilan/Library/Developer/Xcode/DerivedData/test_11_execve_and_file-bdlwbqeghvzabxctoaqsasccjgeb/Build/Products/Debug
test_11_e 91054 liujilan  txt    REG    1,4     69584           147216521 /Users/liujilan/Library/Developer/Xcode/DerivedData/test_11_execve_and_file-bdlwbqeghvzabxctoaqsasccjgeb/Build/Products/Debug/test_11_execve_and_file
test_11_e 91054 liujilan  txt    REG    1,4   2160688 1152921500312781028 /usr/lib/dyld
test_11_e 91054 liujilan  txt    REG    1,4    378384 1152921500312781167 /usr/lib/system/introspection/libsystem_pthread.dylib
test_11_e 91054 liujilan  txt    REG    1,4    324352           143467877 /Applications/Xcode.app/Contents/Developer/usr/lib/libBacktraceRecording.dylib
test_11_e 91054 liujilan  txt    REG    1,4   1393568           143619986 /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/Library/Debugger/libViewDebuggerSupport.dylib
test_11_e 91054 liujilan  txt    REG    1,4    912192           143472870 /Applications/Xcode.app/Contents/SharedFrameworks/DebugHierarchyFoundation.framework/Versions/A/DebugHierarchyFoundation
test_11_e 91054 liujilan  txt    REG    1,4    930528           143467868 /Applications/Xcode.app/Contents/Developer/usr/lib/libMainThreadChecker.dylib
test_11_e 91054 liujilan  txt    REG    1,4   1401344 1152921500312781165 /usr/lib/system/introspection/libdispatch.dylib
test_11_e 91054 liujilan  txt    REG    1,4 803340288 1152921500312725478 /System/Library/dyld/dyld_shared_cache_x86_64h
test_11_e 91054 liujilan    0r   CHR   16,4       0t0                1099 /dev/ttys004
test_11_e 91054 liujilan    1w   CHR   16,4      0t62                1099 /dev/ttys004
test_11_e 91054 liujilan    2u   CHR   16,4       0t0                1099 /dev/ttys004

liujilan@LiuJiLans-MBP-2019 ~ % lsof -p 91057
COMMAND    PID     USER   FD   TYPE DEVICE SIZE/OFF                NODE NAME
test.out 91057 liujilan  cwd    DIR    1,4       96           147211462 /Users/liujilan/Library/Developer/Xcode/DerivedData/test_11_execve_and_file-bdlwbqeghvzabxctoaqsasccjgeb/Build/Products/Debug
test.out 91057 liujilan  txt    REG    1,4    49472           147208341 /Users/liujilan/Desktop/LDC/Competition/2022_OS/test_11_execve_and_file/test_11_execve_and_file/test.out
test.out 91057 liujilan  txt    REG    1,4  2160688 1152921500312781028 /usr/lib/dyld
test.out 91057 liujilan    0r   CHR   16,4      0t0                1099 /dev/ttys004
test.out 91057 liujilan    1w   CHR   16,4     0t62                1099 /dev/ttys004
test.out 91057 liujilan    2u   CHR   16,4      0t0                1099 /dev/ttys004
```

看来execve确实打开了文件, 但好像没有打开路径

# TEMP

```c
struct file {
  enum { FD_NONE, FD_PIPE, FD_INODE, FD_DEVICE } type;
  int ref; // reference count
  char readable;
  char writable;
  struct pipe *pipe; // FD_PIPE
  struct inode *ip;  // FD_INODE and FD_DEVICE
  uint off;          // FD_INODE
  short major;       // FD_DEVICE
};

struct inode {
  uint dev;           // Device number
  uint inum;          // Inode number
  int ref;            // Reference count
  struct sleeplock lock; // protects everything below here
  int valid;          // inode has been read from disk?

  short type;         // copy of disk inode
  short major;
  short minor;
  short nlink;
  uint size;
  uint addrs[NDIRECT+1];
};
```



gcc在远古版本有个-fipa-struct-reorg选项可以结构重排序





## 策略

可以将很多设备放进等待, 但是一定不能几个设备一起等待



# VFS

http://www.science.unitn.it/~fiorella/guidelinux/tlk/node102.html

