//
//  syscall.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/5/12.
//

#include "syscall.h"
#include "hart.h"

extern void panic(char * s);

extern void sys_clone(struct proc * proc);
extern void sys_wait4(struct proc * proc);
extern void sys_exit(struct proc * proc);
extern void sys_getppid(struct proc * proc);
//extern void sys_getpid(void);
extern void sys_shed_yield(struct proc * proc);

extern void sys_brk(struct proc * proc);
extern void sys_uname(struct proc * proc);
extern void sys_times(struct proc * proc);

extern void sys_nanosleep(struct proc * proc);
extern void sys_gettimeofday(struct proc * proc);

void syscall_handler(struct trap_regs * regs, struct proc * proc) {
    regs_t sys_num = proc->context.a7;
    pid_t mypid = proc->pid;
    
    char str[12] = "getpid:0;";  //  FOR DEBUG
    
    //  BUG, 几个用pid为参数的进程可以改用proc指针
    //  这里的传参安排肯定是有问题的
    
    switch (sys_num) {
        case SYS_clone:
            panic("SYS_clone");
            sys_clone(proc);
            break;
        
        case SYS_wait4:
            panic("SYS_wait4");
            sys_wait4(proc);
            break;
            
        case SYS_exit:
            panic("SYS_exit");
            sys_exit(proc);
            break;
            
        case SYS_getppid:
            panic("SYS_getppid");
            sys_getppid(proc);
            break;
            
        case SYS_getpid:
            str[7] += mypid;    //  FOR DEBUG
            panic(str);         //  FOR DEBUG
            proc->context.a0 = mypid;
            proc->context.sepc += 4;
            break;
            
        case SYS_shed_yield:
            panic("SYS_shed_yield");
            sys_shed_yield(proc);
            break;
            
        case SYS_umount:
        case SYS_mount:
            proc->context.a0 = 0;
            proc->context.sepc += 4;
            break;
            
        case SYS_brk:
            panic("SYS_brk");
            sys_brk(proc);
            break;
            
        case SYS_uname:
            panic("SYS_uname");
            sys_uname(proc);
            break;
            
        case SYS_times:
            panic("SYS_times");
            sys_times(proc);
            break;
            
        case SYS_gettimeofday:
            panic("SYS_gettimeofday");
            sys_gettimeofday(proc);
            break;
            
        case SYS_nanosleep:
            panic("SYS_nanosleep");
            sys_nanosleep(proc);
            break;
            
        default:
            panic("UNKNOWN SYSCALL!");
            proc->context.a0 = -1;
            proc->context.sepc += 4;
            break;
    }
}
