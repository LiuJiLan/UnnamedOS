//
//  proc.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/5/5.
//

#include "proc.h"
#include "param.h"
#include "vm.h"
#include "string.h"
#include "kalloc.h"
#include "hart.h"
#include "riscv.h"

extern void panic(char *s);

extern void post_trap_handler(struct trap_regs *);
//  在trap_handler.S中, a0作为恢复CPU上下文的指针

extern char initcode[];
extern char initcode_len[];

struct {
    struct spinlock lock;
    struct proc proctbl[64];
} kproc;

//  注意有两种锁, 一个是进程表的锁, 一个是每个进程(进程表项)的锁
//  1.如果操作(写操作)的对象仅是一个进程,
//  则先获取进程表锁, 再获得目标进程的锁, 然后马上释放进程表锁,
//  对目标进程操作完后释放目标进程的锁
//  要求先获取进程表锁是为了保证 情况2 的原子性
//  2.如果操作(写操作)的对象涉及多个进程,
//  则先获取进程表锁, 再获得每一个需要的目标进程的锁, 然后进行操作
//  进程表锁可以再所有目标进程的锁都被获取后就释放, 也可以等操作结束后再释放
//  获取进程表锁的目的只是为了保障操作的原子性
//  #1  情况2原子性是否必要可以讨论, 但确保其原子性能减少一些编程难度是不言自明的
//  #2  所有的读操作, 尤其是反复遍历寻找的读操作禁止在读操作时对进程表上锁
//      例:在进程表中寻找一个runnable的进程并选中作为本CPU正在运行的进程
//      此例中, 寻找部分不能上锁进程表,
//      (但当改runnable为running时为写操作, 必须上锁)
//      寻找部分不能上锁的原因是:
//      极端情况下, 寻找目标进程的CPU(们)不断成功获取进程表的锁,
//      而能给进程表提供runnable的CPU却一直不幸的获取不了锁
//  #3  情况2下, 如果有必须要解开单个进程的锁才能进行的操作
//      如proc_clear_proc, 则进程表锁应该在所有操作执行完后再释放
//  #4  后续开发中为了防止频繁的上锁, 设计了proc运行时确保私有的变量这一概念
//      对这些变量处理时可以更灵活的处理锁的问题
//      例1:时间片的处理中就没上锁
//      例2:proc_find_runnable_to_run中,
//      一旦找到的进程被确定更改为UNSCHEDULABLE就可以放锁了,
//      因为这已经确保这个进程不会被其他CPU再次修改了
//      例3:处理时间片到的proc_timeout中,
//      时间片的重新赋值可以在上锁之前完成, 减少锁之内的运行时间


//  考虑到后期兼容, 有三个特殊进程
//  PID = 0, idle
//  PID = 1, init
//  PID = 2, kthreadd

//  idle进程由内核加载(其代码来自内核)
//  (本质上所有进程都是内核(态)加载的)
//  我们暂时没有实现内核线程, 所以只是保留了2号PID
//  init进程本质上应该去硬盘中寻找
//  但:
//  由于开发早期文件系统还未成形,
//  我们还是临时把1号进程放在内核代码中
//  另外:
//  由于内核赛第一阶段规定, init也是放在内核代码中的
//  具体方法是(在现在的设计下的):
//  设0号为ZOMBIE态(进程的state = ZOMBIE,
//  sched = UNSCHEDULABLE, unable = UNUSABLE)
//  设2号为ZOMBIE态(进程的state = ZOMBIE,
//  sched = UNSCHEDULABLE, unable = UNUSABLE)
//  由于init进程永不返回, 因此也不用担心出问题,
//  且(在此设计下)0号和2号都没有父进程, 不会变为DEAD

//  注意我们现在的这个进程表是静态的

//  https://www.likecs.com/show-203947168.html

void proc_clear_proc(pid_t pid);
int proc_load_bin(pid_t pid, char* kva_start, size_t len);

void procinit(void) {
    initlock(&kproc.lock, "kproc");
    
    struct proc * p = NULL;
    
    for (int i = 0; i < NPROC; i++) {
        proc_clear_proc(i);
    }
    
    /*
     // 大写开头代表用户的系统调用
     
     // for PID = 0
     if (Fork() == 0) { //  Child
     Execve("/init", ...);
     } else {
     Waitpid();
     printf("如果返回了就真见鬼了!")
     }
     
     // for PID = 1
     pid_t shell_pid = Fork();
     
     if (shell_pid == 0) { //  Child
     Execve("/shell", ...); //  shell程序
     }
     
     while (1) {
     pid_t ret = Waitpid();
     if (ret == shell_pid) {
     pid_t shell_pid = Fork();
     if (shell_pid == 0) { //  Child
     Execve("/shell", ...); //  重开一个shell
     }
     }
     }
     
     */
    
    //  设置0号进程
    //  开发初期特殊情况1号进程也是这时创建的
    
    p = &kproc.proctbl[0];
    p->usable = UNUSABLE;
    p->cpid_bitmap = 0x1 << 1;
    
    p = &kproc.proctbl[2];
    p->usable = UNUSABLE;
    p->ppid = 0;
    
    p = &kproc.proctbl[1];
    p->ppid = 0;
    p->upgtbl = vm_init_upgtbl();
    proc_load_bin(1, initcode, *(uint64 *)initcode_len);
    //  加载二进制形式的程序代码
    //  注意本来是应该检查是否为-1的, 暂时默认一定会成功
    
    p->state = RUNNING;
    p->sched = SCHEDULABLE;
    p->usable = UNUSABLE;
    p->remain_time = DEFAULT_TIME;
}

//  初始化一个进程
//  注意, 页表不会被alloc
//  (后续删除一个进程的时候页表却会被回收)
//  将进程设置为USABLE(也可以说是DEAD)的状态
//  只能用于不带锁的初始化
void proc_clear_proc(pid_t pid) {
    struct proc * p = &kproc.proctbl[pid];
    memset(p, 0, sizeof(struct proc));
    
    p->state = ZOMBIE;
    p->sched = UNSCHEDULABLE;
    p->usable = USABLE;
    
    p->pid = pid;
    
    initlock(&p->lock, "proc");
}

//  将 trap的上下文 复制到 进程的上下文
//  regs为NULL时只需要给context清0
void proc_context_copyin(struct trap_regs * regs, struct context * proc_context) {
    //  in script context
    if (regs) {
        proc_context->ra = regs->ra;
        proc_context->sp = regs->sp;
        proc_context->gp = regs->gp;
        //proc_context->tp = regs->tp;
        proc_context->t0 = regs->t0;
        proc_context->t1 = regs->t1;
        proc_context->t2 = regs->t2;
        proc_context->s0 = regs->s0;
        proc_context->s1 = regs->s1;
        proc_context->a0 = regs->a0;
        proc_context->a1 = regs->a1;
        proc_context->a2 = regs->a2;
        proc_context->a3 = regs->a3;
        proc_context->a4 = regs->a4;
        proc_context->a5 = regs->a5;
        proc_context->a6 = regs->a6;
        proc_context->a7 = regs->a7;
        proc_context->s2 = regs->s2;
        proc_context->s3 = regs->s3;
        proc_context->s4 = regs->s4;
        proc_context->s5 = regs->s5;
        proc_context->s6 = regs->s6;
        proc_context->s7 = regs->s7;
        proc_context->s8 = regs->s8;
        proc_context->s9 = regs->s9;
        proc_context->s10 = regs->s10;
        proc_context->s11 = regs->s11;
        proc_context->t3 = regs->t3;
        proc_context->t4 = regs->t4;
        proc_context->t5 = regs->t5;
        proc_context->t6 = regs->t6;
        proc_context->sepc = regs->sepc;
    } else {
        memset(proc_context, 0, sizeof(struct context));
    }
}

//  将 进程的上下文 复制到 trap的上下文
void proc_context_copyout(struct trap_regs * regs, struct context * proc_context) {
    regs->ra = proc_context->ra;
    regs->sp = proc_context->sp;
    regs->gp = proc_context->gp;
    //regs->tp = proc_context->tp;
    regs->t0 = proc_context->t0;
    regs->t1 = proc_context->t1;
    regs->t2 = proc_context->t2;
    regs->s0 = proc_context->s0;
    regs->s1 = proc_context->s1;
    regs->a0 = proc_context->a0;
    regs->a1 = proc_context->a1;
    regs->a2 = proc_context->a2;
    regs->a3 = proc_context->a3;
    regs->a4 = proc_context->a4;
    regs->a5 = proc_context->a5;
    regs->a6 = proc_context->a6;
    regs->a7 = proc_context->a7;
    regs->s2 = proc_context->s2;
    regs->s3 = proc_context->s3;
    regs->s4 = proc_context->s4;
    regs->s5 = proc_context->s5;
    regs->s6 = proc_context->s6;
    regs->s7 = proc_context->s7;
    regs->s8 = proc_context->s8;
    regs->s9 = proc_context->s9;
    regs->s10 = proc_context->s10;
    regs->s11 = proc_context->s11;
    regs->t3 = proc_context->t3;
    regs->t4 = proc_context->t4;
    regs->t5 = proc_context->t5;
    regs->t6 = proc_context->t6;
    regs->sepc = proc_context->sepc;
}


//  以二进制的形式向进程加载代码
//  要求: 进程的upgtbl必须已经初始化过
//  如果采用这种方式加载, 那么ENTRY默认在0x1000
int proc_load_bin(pid_t pid, char* kva_start, size_t len) {
    struct proc * proc = &kproc.proctbl[pid];
    pgtbl_t upgtbl = proc->upgtbl;
    
    uptr_t DEFAULT_ENTRY = 0x1000U;
    
    uptr_t uva_pg_start = PGROUNDDOWN(DEFAULT_ENTRY);
    uptr_t uva_pg_end = PGROUNDUP(uva_pg_start + len);
    
    for (uptr_t uva_pg = uva_pg_start; uva_pg < uva_pg_end; uva_pg += PGSIZE) {
        uptr_t kva = (uptr_t)kalloc();
        if (!kva) {
            return -1;
        }
        vm_kva_map_uva(upgtbl, kva, uva_pg);
    }
    
    //  映射用户栈
    uptr_t kva = (uptr_t)kalloc();
    if (!kva) {
        return -1;
    }
    vm_kva_map_uva(upgtbl, kva, MAXUVA - PGSIZE);
    
    int ret = vm_memmove(upgtbl, (uptr_t)kva_start, 0x1000U, len, 0);
    if (ret == -1) {
        return ret;
    }
    
    proc->PROC_ENTRY = 0x1000U;
    proc->PROC_END = proc->PROC_ENTRY + len;
    proc->PROC_BRK = proc->PROC_END;
    proc->PROC_CODE_PG = uva_pg_start;
    proc->PROC_CODE_SZ = (int)((uva_pg_end - uva_pg_start) / PGSIZE);
    
    proc->PROC_STACK_TOP = (MAXUVA - 1) & -16;    //  模仿u-boot
    
    proc->context.sepc = proc->PROC_ENTRY;
    proc->context.sp = proc->PROC_STACK_TOP;
    
    return 0;
}




//  --------------------------
//  以下这些函数是给proc外部的文件使用的
//  为的是不暴露kproc
//  有些函数在proc中完全有方法解决
//  这样的情况就不要再用以下函数包一层了



//  每个CPU在第一次运行进程之前应该临时的给出一个regs的结构体
//  注意, 应该确保这个trap_regs结构体的生命周期
void pre_first_run_proc(struct trap_regs * regs) {
    regs->tp = r_tp();
    regs_t sstatus = r_sstatus();
    sstatus |= 0x1U << 5;    //  SPIE, 开中断
    sstatus &= ~(0x1U << 8);    //  SPP = USER
    regs->sstatus = sstatus;
}

//  由于我们暂时采取的是不断的按顺序寻找
//  pid用于避免较小pid号的刚刚被标为RUNNABLE的进程马上被重新唤醒
//  而导致较大的pid号的进程不容易被选中的情况
void proc_find_runnable_to_run(struct trap_regs * regs, pid_t pid) {
    struct proc * p = &kproc.proctbl[0];
    
    //  第一次循环
    for (int i = (pid + 1) % NPROC; i < NPROC; i++) {
        if ((p + i)->sched == SCHEDULABLE) {
            acquire(&kproc.lock);
            acquire(&kproc.proctbl[i].lock);
            release(&kproc.lock);
            if ((p + i)->sched == SCHEDULABLE) {
                //  带锁后重新确认
                
                (p + i)->sched = UNSCHEDULABLE;
                release(&kproc.proctbl[i].lock);
                //  对单进程做完写操作就可以放锁了
                
                my_hart()->myproc = (p + i);
                proc_context_copyout(regs, &(p + i)->context);
                vm_2_proc_upgtbl((p + i)->upgtbl);
                
                //  重制一下时间
                sbi_set_timer(DEFAULT_SBI_TIMER);
                
                post_trap_handler(regs);
            } else {
                continue;
            }
        }
    }
    
    //  死循环, 有合适的就会跳出, 不会有真正的返回
    //  因为trap的上下文被改写了
    while (1) {
        for (int i = 0; i < NPROC; i++) {
            if ((p + i)->sched == SCHEDULABLE) {
                acquire(&kproc.lock);
                acquire(&kproc.proctbl[i].lock);
                release(&kproc.lock);
                if ((p + i)->sched == SCHEDULABLE) {
                    (p + i)->sched = UNSCHEDULABLE;
                    release(&kproc.proctbl[i].lock);
                    my_hart()->myproc = (p + i);
                    proc_context_copyout(regs, &(p + i)->context);
                    vm_2_proc_upgtbl((p + i)->upgtbl);
                    sbi_set_timer(DEFAULT_SBI_TIMER);
                    post_trap_handler(regs);
                } else {
                    continue;
                }
            }
        }
    }
}

//  注意不是fork系统调用
//  要求两个进程已经被上锁
//  成功返回0, 失败返回-1
//  (清理工作应该在ZOMBIE -> DEAD的阶段就完成)
int proc_fork(pid_t ppid, pid_t cpid) {
    struct proc * pproc = &kproc.proctbl[ppid];
    struct proc * cproc = &kproc.proctbl[cpid];
    
    cproc->usable = UNUSABLE;
    //  早些更改状态, 其他CPU读到后尝试上锁(只是节约时间)
    
    //  设置好父子进程关系
    cproc->ppid = ppid;
    pproc->cpid_bitmap |= 0x1U << cpid;
    
    cproc->remain_time = DEFAULT_TIME;
    
    cproc->context = pproc->context;
    
    cproc->upgtbl = vm_init_upgtbl();
    if (cproc->upgtbl == NULL) {
        return -1;
    }
    int ret = vm_deep_copy(cproc->upgtbl, pproc->upgtbl);
    if (ret == -1) {
        return -1;
    }
    
    cproc->PROC_ENTRY = pproc->PROC_ENTRY;
    cproc->PROC_END = pproc->PROC_END;
    cproc->PROC_BRK = pproc->PROC_BRK;
    cproc->PROC_CODE_PG = pproc->PROC_CODE_PG;
    cproc->PROC_CODE_SZ = pproc->PROC_CODE_SZ;
    
    cproc->PROC_STACK_TOP = pproc->PROC_STACK_TOP;
    
    cproc->state = RUNNING;
    cproc->sched = SCHEDULABLE;
    
    return 0;
}

void proc_acquire_proctbl_lock(void) {
    acquire(&kproc.lock);
}

void proc_release_proctbl_lock(void) {
    release(&kproc.lock);
}

void proc_acquire_proc_lock(pid_t pid) {
    acquire(&kproc.proctbl[pid].lock);
}

void proc_release_proc_lock(pid_t pid) {
    release(&kproc.proctbl[pid].lock);
}

void proc_timeout(pid_t pid) {
    kproc.proctbl[pid].remain_time = DEFAULT_TIME;
    
    acquire(&kproc.lock);
    acquire(&kproc.proctbl[pid].lock);
    release(&kproc.lock);
    
    kproc.proctbl[pid].sched = SCHEDULABLE;
    
    release(&kproc.proctbl[pid].lock);
}
