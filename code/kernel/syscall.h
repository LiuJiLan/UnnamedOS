//
//  syscall.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/5/12.
//

#ifndef syscall_h
#define syscall_h

#include "trap.h"

void syscall_handler(struct trap_regs * regs);

#endif /* syscall_h */
