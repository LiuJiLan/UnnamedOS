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
#define SYS_shed_yield 124

#define SYS_umount  39
#define SYS_mount   40

#define SYS_brk 214
#define SYS_uname 160
#define SYS_times 153

#define SYS_nanosleep 101
#define SYS_gettimeofday 169

#define SYS_read 63
#define SYS_write 64

void syscall_handler(struct trap_regs * regs, struct proc * proc);

#endif /* syscall_h */
