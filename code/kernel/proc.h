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

typedef int pid_t;

enum proc_state {RUNNING, INTERRUPTIBLE, ZOMBIE};
//  暂时使用枚举来实现, 表示进程的状态, 暂时没有UNINTERRUPTIBLE
//  ZOMBIE和DEAD的区分用其他来实现

enum proc_sched {UNSCHEDULABLE, SCHEDULABLE};
//  是否能被CPU调度, 只在CPU寻找所要的进程时使用

enum proc_usable {UNUSABLE, USABLE};
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
    volatile enum proc_state state;
    volatile enum proc_sched sched;
    volatile enum proc_usable usable;
    
    pid_t pid;              //  只是为了快速读取本进程的pid
    pid_t ppid;             //  父进程
    uint64 cpid_bitmap;     //  子进程bitmap
    
    uint64 zombie_bitmap;   //  zombie子进程集合
    
    int xstate;         //  返回状态, 用于父进程
    
    int remain_time;    //  剩余时间片, 用于调度
    
    struct context context;
    pgtbl_t upgtbl;
    
    ptr_t PROC_ENTRY;    //  code的实际entry地址
    ptr_t PROC_END;      //  code的实际结尾
    ptr_t PROC_BRK;      //  brk的位置
    ptr_t PROC_CODE_PG;  //  code分配的页起始地址
    int PROC_CODE_SZ;    //  code分配了都少个页
    
    ptr_t PROC_STACK_TOP;//  用户程序栈顶
    //int PROC_STACK_SZ;    //  栈用了多少页, 我们暂时不设置这个值
    
    struct spinlock lock;
};

void pre_first_run_proc(struct trap_regs * regs);
void procinit(void);

void proc_context_copyin(struct trap_regs * regs, struct context * proc_context);
void proc_context_copyout(struct trap_regs * regs, struct context * proc_context);
int proc_load_bin(pid_t pid, char* kva_start, size_t len);
void proc_find_runnable_to_run(struct trap_regs * regs, pid_t pid);

void proc_acquire_proctbl_lock(void);
void proc_release_proctbl_lock(void);
void proc_acquire_proc_lock(pid_t pid);
void proc_release_proc_lock(pid_t pid);

#endif /* proc_h */
