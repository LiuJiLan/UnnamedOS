//
//  syscall.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/5/12.
//

#include "syscall.h"
#include "hart.h"

extern void panic(char * s);

extern void sys_clone(pid_t pid);
extern void sys_wait4(struct trap_regs * regs, pid_t pid);
extern void sys_exit(struct trap_regs * regs, pid_t pid);
extern void sys_getppid(pid_t pid);
//extern void sys_getpid(void);

void syscall_handler(struct trap_regs * regs, struct proc * proc) {
    regs_t sys_num = proc->context.a7;
    pid_t mypid = proc->pid;
    
    char str[10] = "mypid:0;";  //  FOR DEBUG
    
    switch (sys_num) {
        case SYS_clone:
            sys_clone(mypid);
            break;
        
        case SYS_wait4:
            sys_wait4(regs, mypid);
            break;
            
        case SYS_exit:
            panic("EXIT");
            sys_exit(regs, mypid);
            panic("EXIT_2");
            break;
            
        case SYS_getppid:
            sys_getppid(mypid);
            break;
            
        case SYS_getpid:
            str[6] += mypid;
            panic(str);
            proc->context.a0 = mypid;
            proc->context.sepc += 4;
            break;
            
        default:
            panic("UNKNOWN SYSCALL!");
            proc->context.a0 = -1;
            proc->context.sepc += 4;
            break;
    }
}
