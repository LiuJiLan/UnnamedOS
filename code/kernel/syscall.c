//
//  syscall.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/5/12.
//

#include "syscall.h"
#include "hart.h"
#include "proc.h"
#include "types.h"

extern void panic(char * s);

#define SYS_clone 220
#define SYS_wait4 260
#define SYS_exit 93
#define SYS_getppid 173
#define SYS_getpid 172

extern void sys_clone(struct trap_regs * regs, pid_t pid);
extern void sys_wait4(void);
extern void sys_exit(void);
extern void sys_getppid(void);
extern void sys_getpid(void);

void syscall_handler(struct trap_regs * regs) {
    regs_t sys_num = my_hart()->myproc->context.a7;
    pid_t mypid = my_hart()->myproc->pid;
    
    switch (sys_num) {
        case SYS_clone:
            sys_clone(regs, mypid);
            break;
            
        default:
            panic("UNKNOWN SYSCALL!");
            regs->sepc += 4;
            break;
    }
}
