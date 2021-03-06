//
//  proc.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/5/5.
//

#ifndef proc_h
#define proc_h

#include "types.h"
#include "vm.h"
#include "spinlock.h"
#include "trap.h"
#include "param.h"

typedef int pid_t;

//  enum proc_state {RUNNING, INTERRUPTIBLE, ZOMBIE};
//  暂时使用枚举来实现, 表示进程的状态, 暂时没有UNINTERRUPTIBLE
//  ZOMBIE和DEAD的区分用其他来实现

//  enum proc_sched {UNSCHEDULABLE, SCHEDULABLE};
//  是否能被CPU调度, 只在CPU寻找所要的进程时使用

//  enum proc_usable {UNUSABLE, USABLE};
//  只在新建进程的时候读并更改

struct context {
    unsigned long ra;
    unsigned long sp;
    unsigned long gp;
    //unsigned long tp;
    unsigned long t0;
    unsigned long t1;
    unsigned long t2;
    unsigned long s0;
    unsigned long s1;
    unsigned long a0;
    unsigned long a1;
    unsigned long a2;
    unsigned long a3;
    unsigned long a4;
    unsigned long a5;
    unsigned long a6;
    unsigned long a7;
    unsigned long s2;
    unsigned long s3;
    unsigned long s4;
    unsigned long s5;
    unsigned long s6;
    unsigned long s7;
    unsigned long s8;
    unsigned long s9;
    unsigned long s10;
    unsigned long s11;
    unsigned long t3;
    unsigned long t4;
    unsigned long t5;
    unsigned long t6;
    unsigned long sepc; //  相当于保存了pc
}__attribute__((packed));

struct fde {    //  file descriptor entry
    struct file * file;
    enum {FDE_CLOSE = 0, FDE_OPEN} isOpened;
    int flags;  //  open中的flags会被放入这里
    //int modes;  //  open中的modes
};

//  与状态有关的必须都是volatile变量
//  虽然除此之外的变量也有可能被多个CPU改变
//  但是我们对kproc上的锁可以保证
//  这些变量在第一次读与后续的写回期间不会被其他CPU更改
//
//  但是有关状态的变量我们有可能会在获取锁前读一次,
//  然后根据这次读的结果去判断有没有必要去尝试它
//  (如果粗读的结果就不是我们想要的我们就没有必要上锁再做二次确认)
//  在第一次读取和上锁后的第二次读之间, 不能确保其不被其他CPU更改
//  所以一定不能使用寄存器中的值, 需要用volatile来确保
struct proc {
    volatile enum {RUNNING, INTERRUPTIBLE, ZOMBIE} state;
    volatile enum {UNSCHEDULABLE, SCHEDULABLE} sched;
    volatile enum {UNUSABLE, USABLE} usable;
    
    pid_t pid;              //  只是为了快速读取本进程的pid //#
    pid_t ppid;             //  父进程
    uint64 cpid_bitmap;     //  子进程bitmap
    
    uint64 zombie_bitmap;   //  zombie子进程集合
    
    int xstate;         //  返回状态, 用于父进程 //#
    
    int remain_time;    //  剩余时间片, 用于调度 //#
    
    struct context context; //#
    pgtbl_t upgtbl;
    
    uptr_t PROC_ENTRY;    //  code的实际entry地址
    uptr_t PROC_END;      //  code的实际结尾
    uptr_t PROC_BRK;      //  brk的位置
    uptr_t PROC_CODE_PG;  //  code分配的页起始地址
    int PROC_CODE_SZ;    //  code分配了都少个页
    
    uptr_t PROC_STACK_TOP;   //  用户程序栈顶
    uptr_t PROC_STACK_PG;    //  用户栈起始位置(低地址)
    int PROC_STACK_SZ;      //  栈用了多少页, 我们暂时不设置这个值
    
    //  给proc_sleep_for_reason和proc_wakeup_for_reason函数使用的
    //  注意例如exit中判断父进程的状态的时候不应该使用这个为依据
    void * sleep_reason;
    
    struct fde fdtbl[NFD];
    
    struct spinlock lock;
};
//  标#的条目是被认为这个进程一旦被创立就一定会是私有的量
//  由于对进程的写操作是一定要上锁的(部分变量也要上锁读)
//  但暂时的设计是想要上单个进程的锁就需要给整个表先上锁(虽然很快就会释放)
//  但本身抢占锁本身就是一件有些浪费时间的事, 所以我们设计了一些变量
//  (可能不止标出的这些)只要判定他们不会发生多CPU冲突, 就可以不上锁的处理
//  例如:
//  ppid就是一个读写都需上锁的变量
//  因为父进程变为ZOMBIE时, 子进程有可能变为孤儿进程,
//  所以在(父进程的)exit系统调用中父进程会更改所有子进程的ppid

void procinit(void);

void proc_context_copyin(struct trap_regs * regs, struct context * proc_context);
void proc_context_copyout(struct trap_regs * regs, struct context * proc_context);

void proc_find_runnable_to_run(pid_t pid);
void proc_reschedule(pid_t pid);

void proc_acquire_proctbl_lock(void);
void proc_release_proctbl_lock(void);
void proc_acquire_proc_lock(pid_t pid);
void proc_release_proc_lock(pid_t pid);

void proc_timeout(pid_t pid);

void proc_sleep_proc(pid_t pid);
void proc_wakeup_proc(pid_t pid);
void proc_sleep_for_reason(struct spinlock * lk, void * reason);
void proc_wakeup_for_reason(void * reason);

void proc_handle_syscall(struct trap_regs * regs, pid_t pid);

#endif /* proc_h */
