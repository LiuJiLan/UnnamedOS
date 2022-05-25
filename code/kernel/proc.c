//
//  proc.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/5/5.
//

#include "proc.h"
#include "vm.h"
#include "string.h"
#include "kalloc.h"
#include "hart.h"
#include "riscv.h"
#include "syscall.h"
#include "fd.h"
#include "file.h"

extern void panic(char *s);

extern void post_trap_handler(struct trap_regs *);
//  在trap_handler.S中, a0作为恢复CPU上下文的指针

extern char initcode[];
extern char initcode_len[];

struct {
    struct spinlock lock;
    struct proc proctbl[NPROC];
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
    
    //  STDIN 0
    p->fdtbl[0].isOpened = FDE_OPEN;
    p->fdtbl[0].flags = O_RDONLY;
    p->fdtbl[0].file = &kftable.filetbl[0];
    
    //  STDOUT 1
    p->fdtbl[1].isOpened = FDE_OPEN;
    p->fdtbl[1].flags = O_WRONLY;
    p->fdtbl[1].file = &kftable.filetbl[0];
    
    //  STDERR 2
    p->fdtbl[2].isOpened = FDE_OPEN;
    p->fdtbl[2].flags = O_WRONLY;
    p->fdtbl[2].file = &kftable.filetbl[0];
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

void proc_delete_proc(pid_t pid) {
    struct proc * p = &kproc.proctbl[pid];
    vm_delete_upgtbl(p->upgtbl);
    proc_clear_proc(pid);
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
//  regs给NULL时不操作
void proc_context_copyout(struct trap_regs * regs, struct context * proc_context) {
    if (regs) { //  NULL是假系统调用
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
    proc->PROC_STACK_SZ = 1;
    proc->PROC_STACK_PG = MAXUVA - PGSIZE * 1;
    
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
void proc_init_trap_context(struct trap_regs * regs) {
    regs->tp = r_tp();
    regs_t sstatus = r_sstatus();
    sstatus |= SSTATUS_SPIE;    //  SPIE, 开中断
    sstatus &= ~(SSTATUS_SPP);    //  SPP = USER
    regs->sstatus = sstatus;
}

//  由于我们暂时采取的是不断的按顺序寻找
//  pid用于避免较小pid号的刚刚被标为RUNNABLE的进程马上被重新唤醒
//  而导致较大的pid号的进程不容易被选中的情况
void proc_find_runnable_to_run(pid_t pid) {
    struct proc * p = &kproc.proctbl[0];
    struct trap_regs regs;  //  注意其实不需要传入regs了, 因为该保存的都保存了
    proc_init_trap_context(&regs);
    
    //  第一次循环
    for (int i = (pid + 1) % NPROC; i < NPROC; i++) {
        if ((p + i)->sched == SCHEDULABLE) {
            intr_off();
            acquire(&kproc.lock);
            acquire(&(p + i)->lock);
            release(&kproc.lock);
            if ((p + i)->sched == SCHEDULABLE) {
                //  带锁后重新确认
                
                (p + i)->sched = UNSCHEDULABLE;
                release(&(p + i)->lock);
                //  对单进程做完写操作就可以放锁了
                
                set_myproc(p + i);
                proc_context_copyout(&regs, &(p + i)->context);
                vm_2_proc_upgtbl((p + i)->upgtbl);
                
                //  重制一下时间
                sbi_set_timer(DEFAULT_INTERVAL);
                
                post_trap_handler(&regs);
            } else {
                release(&(p + i)->lock);
                intr_on();
                continue;
            }
        }
    }
    
    //  死循环, 有合适的就会跳出, 不会有真正的返回
    //  因为trap的上下文被改写了
    while (1) {
        for (int i = 0; i < NPROC; i++) {
            if ((p + i)->sched == SCHEDULABLE) {
                intr_off();
                acquire(&kproc.lock);
                acquire(&(p + i)->lock);
                release(&kproc.lock);
                if ((p + i)->sched == SCHEDULABLE) {
                    (p + i)->sched = UNSCHEDULABLE;
                    release(&(p + i)->lock);
                    set_myproc(p + i);
                    proc_context_copyout(&regs, &(p + i)->context);
                    vm_2_proc_upgtbl((p + i)->upgtbl);
                    sbi_set_timer(DEFAULT_INTERVAL);
                    post_trap_handler(&regs);
                } else {
                    release(&(p + i)->lock);
                    intr_on();
                    continue;
                }
            }
        }
    }
}

void proc_reschedule(pid_t pid) {
    vm_2_kpgtbl();
    set_myproc(NULL);
    
    //  如果不在这里设置一个时钟中断,
    //  intr_on()会被打断
    //  然后在一次从S态陷入的STIP之后
    //  sret返回之后两次ret时候,
    //  ra会指向错误的位置(位于数据段中栈的位置)
    //  一直找不出为什么会出错
    //  只好避免intr_on()被打断
    sbi_set_timer(DEFAULT_INTERVAL);
    intr_on();
    
    proc_find_runnable_to_run(pid);
}

//  注意不是fork系统调用
//  要求两个进程已经被上锁
//  成功返回0, 失败返回-1
//  (清理工作应该在ZOMBIE -> DEAD的阶段就完成)
int proc_fork(pid_t ppid, pid_t cpid) {
    struct proc * pproc = &kproc.proctbl[ppid];
    struct proc * cproc = &kproc.proctbl[cpid];
    
    //  cproc->usable = UNUSABLE;
    //  早些更改状态, 其他CPU读到后尝试上锁(只是节约时间)
    //  在寻找的时候已经改变
    
    //  设置好父子进程关系
    cproc->ppid = ppid;
    pproc->cpid_bitmap |= 0x1U << cpid;
    
    cproc->remain_time = DEFAULT_TIME;
    
    cproc->context = pproc->context;
    
    cproc->upgtbl = vm_init_upgtbl();
    if (cproc->upgtbl == NULL) {
        return -1;
    }
    int ret = vm_deep_copy(cproc->upgtbl, pproc->upgtbl, pproc->PROC_CODE_PG, pproc->PROC_CODE_SZ);
    if (ret == -1) {
        return -1;
    }
    ret = vm_deep_copy(cproc->upgtbl, pproc->upgtbl, pproc->PROC_STACK_PG, pproc->PROC_STACK_SZ);
    if (ret == -1) {
        return -1;
    }
    
    cproc->PROC_ENTRY = pproc->PROC_ENTRY;
    cproc->PROC_END = pproc->PROC_END;
    cproc->PROC_BRK = pproc->PROC_BRK;
    cproc->PROC_CODE_PG = pproc->PROC_CODE_PG;
    cproc->PROC_CODE_SZ = pproc->PROC_CODE_SZ;
    
    cproc->PROC_STACK_TOP = pproc->PROC_STACK_TOP;
    cproc->PROC_STACK_PG = pproc->PROC_STACK_PG;
    cproc->PROC_STACK_SZ = pproc->PROC_STACK_SZ;
    
    cproc->state = RUNNING;
    cproc->sched = SCHEDULABLE;
    
    fd_fork(pproc->fdtbl, cproc->fdtbl);
    
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

//  要求不要对表和本进程上锁
//  如果要求睡眠的是自己CPU拥有的进程
//  则休眠并找到一个新的进程
//  如果不是, 则说明这个sleep是由于中断处理提起,
//  但是任然没有满足的情况则什么都不做
void proc_sleep_proc(pid_t pid) {
    acquire(&kproc.lock);
    acquire(&kproc.proctbl[pid].lock);
    release(&kproc.lock);
    
    if (my_proc()->pid != pid) {
        kproc.proctbl[pid].state = INTERRUPTIBLE;
        release(&kproc.proctbl[pid].lock);
        
        proc_reschedule(pid);
    }
    
    release(&kproc.proctbl[pid].lock);
}

//  要求不要对表和本进程上锁
//  如果wakeup的进程不是自己CPU的进程
//  (由中断处理唤起的)变为RUNNABLE
//  否则不改变
//
//  之前的设计中STIP发生只能是从U态下陷
//  为了设备与时间管理, STIP也有可能发生在S态
//  这个时候CPU可能没有拥有进程(myproc = NULL)
//  例如正在寻找可用进程的CPU也能被时钟中断
//  而时钟中断可能会调用time_ring_clock从而调用到本函数
//  所以要加上对NULL的处理
void proc_wakeup_proc(pid_t pid) {
    acquire(&kproc.lock);
    acquire(&kproc.proctbl[pid].lock);
    release(&kproc.lock);
    
    struct proc * myproc = my_proc();
    
    if (!myproc || myproc->pid != pid) {
        //  myproc == NULL || myproc->pid != pid
        //  注意用短路来避免myproc为NULL时被访问
        kproc.proctbl[pid].state = RUNNING;
        kproc.proctbl[pid].sched = SCHEDULABLE;
    }
    
    release(&kproc.proctbl[pid].lock);
}
//  不能放弃这个思路, 虽然之后一定能找到更好的解决方法

//  学习了xv6的sleep
//  需要传入一个自旋锁
//  其设计是想要完成一个操作但不能及时的完成, 只能进入睡眠
//  注意,
//  1.锁不是造成睡眠的原因
//  2.一个锁下面可能会有多个原因
//  3.只能在这个函数中放自旋锁,
//  因为要保证 拥锁-成果-放锁 和 拥锁-不满足、进程休眠-放锁 这两种动作的原子性
void proc_sleep_for_reason(struct spinlock * lk, void * reason) {
    struct proc * myproc = my_proc();
    
    if (myproc) {
        acquire(&kproc.lock);
        acquire(&myproc->lock);
        release(&kproc.lock);
        
        release(lk);
        
        myproc->sleep_reason = reason;
        myproc->state = INTERRUPTIBLE;
        pid_t mypid = myproc->pid;
        
        release(&myproc->lock);
        
        proc_reschedule(mypid);
    } else {
        release(lk);
    }
}

void proc_wakeup_for_reason(void * reason) {
    struct proc * p = &kproc.proctbl[0];
    
    acquire(&kproc.lock);
    
    //  FOR DEBUG
    char str[30] = "0_proc_wakeup_for_reason";
    
    struct proc * myproc = my_proc();
    for (int i = 0; i < NPROC; i++) {
        if ((p + i) != myproc) {
            acquire(&(p + i)->lock);
            if ((p + i)->state == INTERRUPTIBLE && (p + i)->sleep_reason == reason) {
                (p + i)->sleep_reason = NULL;
                (p + i)->state = RUNNING;
                (p + i)->sched = SCHEDULABLE;
                
                //  FOR DEBUG
                str[0] = '0' + (p + i)->pid;
                panic(str);
            }
            release(&(p + i)->lock);
        }
    }
    
    release(&kproc.lock);
}

//  系统调用的时候可以反复使用
void proc_handle_syscall(struct trap_regs * regs, pid_t pid) {
    //  如果是中断后重新运行的系统调用
    //  regs给NULL
    //  注意, 假系统调用只能用于外设的处理上
    //  比如exit去通知wait4的进程, wait4系统调用中就不需要wakeup自己
    
    //  注意, 一定是从进程的context中取值;
    struct proc * p = &kproc.proctbl[pid];
    syscall_handler(regs, p);
    
    proc_context_copyout(regs, &p->context);
}

//  要求必须给进程表上锁, 且调用这个函数之前的函数自身不应该上锁单个进程
//  (单个进程可能会被其他CPU占有, 但这中情况是没有问题的)
//  如果找到会标UNUSABLE
//  返回0表示没有合适的进程
pid_t proc_find_usable_to_use(void) {
    struct proc * p = &kproc.proctbl[0];
    for (int i = 1; i < NPROC; i++) {
        if ( (p + i)->usable == USABLE ) {
            acquire(&(p + i)->lock);    //  上锁再次检查
            if ( (p + i)->usable == USABLE ) {
                (p + i)->usable = UNUSABLE;
                release(&(p + i)->lock);
                return i;
            }
            release(&(p + i)->lock);
            continue;
        }
    }
    return 0;
}

 
void sys_clone(struct proc * pproc) {
    acquire(&kproc.lock);
    
    pid_t ppid = pproc->pid;
    //  这里的parent proc指的是自己
    
    pid_t cpid = proc_find_usable_to_use();
    
    if (cpid == 0) {
        release(&kproc.lock);
        pproc->context.a0 = -1;
        pproc->context.sepc += 4;
        return;
    }
    
    struct proc * cproc = &kproc.proctbl[cpid];
    acquire(&pproc->lock);
    acquire(&cproc->lock);
    
    int ret = proc_fork(ppid, cpid);
    
    if (ret == -1) {
        release(&pproc->lock);
        release(&cproc->lock);
        proc_delete_proc(cpid);
        
        release(&kproc.lock);
        pproc->context.a0 = -1;
        pproc->context.sepc += 4;
        return;
    }
    
    release(&kproc.lock);
    pproc->context.a0 = cpid;
    pproc->context.sepc += 4;
    cproc->context.a0 = 0;
    cproc->context.sepc += 4;
    release(&pproc->lock);
    release(&cproc->lock);
}

//  有满足的返回pid, 没有则返回0
pid_t proc_waiting_set_satisfied(uint64 zombie_bitmap, uint64 waiting_set) {
    if (waiting_set != -1) {
        zombie_bitmap &= 0x1U << waiting_set;
    }
    for (int i = 1; i < NPROC; i++) {
        if (zombie_bitmap & 0x1U << i) {
            return i;
        }
    }
    return 0;
}

#define WNOHANG         0x00000001  //  0b001
#define WUNTRACED       0x00000002  //  0b010

void sys_wait4(struct proc * pproc) {
    pid_t ppid = pproc->pid;
    //  这里的parent proc指的是自己
    
    if (pproc->cpid_bitmap == 0) {
        pproc->context.a0 = -1;
        pproc->context.sepc += 4;
        return;
    }
    //  注意这个判断连表都无需上锁
    //  一个进程子进程的出现,
    //  要么是自己fork出来的, 要么是自己fork出来的子进程托管的孤儿进程
    //  如果是前者, 那我现在在wait4, 自然不会fork操作
    //  如果是后者, 无论其他CPU有没有写回, 子进程列表一定不为空
    //  由此可知道, 这个判断不需要上任何锁
    
    acquire(&kproc.lock);
    acquire(&pproc->lock);
    //  一旦这两个锁上成功了, 就说明不会再有子进程ZOMBIE表的更改
    
    
    pid_t cpid = proc_waiting_set_satisfied(pproc->zombie_bitmap, pproc->context.a0);
    
    //  FOR DEBUG
    char str[10] = "wait:0;";
    str[5] += cpid;
    panic(str);
    
    if (cpid == 0) {
        if (pproc->context.a2 & WNOHANG) {
            release(&kproc.lock);
            release(&pproc->lock);
            pproc->context.a0 = 0;
            pproc->context.sepc += 4;
            return;
        }
        
        release(&kproc.lock);
        pproc->state = INTERRUPTIBLE;
        release(&pproc->lock);
        
        proc_reschedule(ppid);
        //  特殊方式的转跳
    }
    
    struct proc * cproc = &kproc.proctbl[cpid];
    acquire(&cproc->lock);
    
    //  FOR DEBUG
    char str2[10] = "kill:0;";
    str2[5] += cpid;
    panic(str2);
    
    uptr_t uva = pproc->context.a1;
    if (uva) {  //  wstatus != NULL
        //  不要自己写函数, 因为uva可能跨越页
        //  BUG!!!!!!!!!!
        //  忽略了vm_memmove的返回!!!
        uptr_t cast_xstate = cproc->xstate;
        vm_memmove(pproc->upgtbl, (uptr_t)&cast_xstate, uva, sizeof(uptr_t), 0);
    }
    
    if (pproc->context.a2 & WUNTRACED) {
        //  只返回不清理
        release(&cproc->lock);
    } else {
        release(&cproc->lock);
        proc_delete_proc(cpid);
        uint64 clear_child_bitmap = ~(0x1U << cpid);
        pproc->cpid_bitmap &= clear_child_bitmap;
        pproc->zombie_bitmap &= clear_child_bitmap;
    }
    
    
    release(&kproc.lock);   //  放进程表锁
    release(&pproc->lock);
    
    pproc->context.a0 = cpid;
    pproc->context.sepc += 4;
    return;
}

void sys_exit(struct proc * proc) {
    acquire(&kproc.lock);
    
    pid_t pid = proc->pid;
    acquire(&proc->lock);   //  先上自己的锁
    
    pid_t ppid = proc->ppid;
    struct proc * pproc = &kproc.proctbl[ppid];
    acquire(&pproc->lock);   //  再上自己父进程的锁
    
    struct proc * p = &kproc.proctbl[0];
    uint64 cpid_bitmap = proc->cpid_bitmap;
    for (int i = 0; i < NPROC; i++) {
        if (cpid_bitmap & 0x1U << i) {  //  是自己的子进程
            acquire( &(p + i)->lock );
            (p + i)->ppid = ppid;
            release( &(p + i)->lock );
        }
    }
    
    release(&kproc.lock);   //  放进程表
    
    proc->state = (int)proc->context.a0;
    pproc->cpid_bitmap |= proc->cpid_bitmap;
    pproc->zombie_bitmap |= proc->zombie_bitmap;
    pproc->zombie_bitmap |= 0x1U << pid;
    
    proc->state = ZOMBIE;
    fd_clear(proc->fdtbl);
    release(&proc->lock);   //  放自己的锁
    
    if (pproc->state == INTERRUPTIBLE && pproc->context.a7 == SYS_wait4) {
        pproc->state = RUNNING;
        pproc->sched = SCHEDULABLE;
    }
    //  否则什么都不用多做, 因为最终的父进程init一定会运行到wait去回收的
    
    release(&pproc->lock);
    
    proc_reschedule(pid);
}

void sys_getppid(struct proc * proc) {
    acquire(&kproc.lock);
    
    acquire(&proc->lock);
    release(&kproc.lock);
    
    proc->context.a0 = proc->ppid;
    proc->context.sepc += 4;
    release(&proc->lock);
    
    return;
}

void sys_shed_yield(struct proc * proc) {
    acquire(&kproc.lock);
    
    pid_t pid = proc->pid;
    acquire(&proc->lock);
    release(&kproc.lock);
    
    proc->context.a0 = 0;
    proc->context.sepc += 4;
    proc->sched = SCHEDULABLE;
    release(&proc->lock);
    
    proc_reschedule(pid);
}
