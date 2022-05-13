//
//  syscall.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/5/12.
//

#ifndef syscall_h
#define syscall_h

#include "trap.h"
#include "proc.h"
#include "types.h"

#define SYS_clone 220
#define SYS_wait4 260
#define SYS_exit 93
#define SYS_getppid 173
#define SYS_getpid 172

void syscall_handler(struct trap_regs * regs, struct proc * proc);

#endif /* syscall_h */
