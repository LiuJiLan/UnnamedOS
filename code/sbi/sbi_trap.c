//
//  sbi_trap.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/27.
//

#include "sbi_trap.h"

struct sbi_trap_regs * sbi_trap_handler(struct sbi_trap_regs * regs) {
    if (regs->a7 == 0x1919 && regs->a6 == 0x810) {
        regs->a0 = 0x114;
        regs->a1 = 0x514;
    }
    
    regs->mepc += 4;
    
    return regs;
}

