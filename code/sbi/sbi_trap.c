//
//  sbi_trap.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/27.
//

#include "sbi_trap.h"
#include "sbi_type.h"
#include "sbi_ecall.h"

struct sbi_trap_regs * sbi_trap_handler(struct sbi_trap_regs * regs) {
    regs_t mcause = regs->mcause;
    
    if (mcause & (0x1UL << 63)) {   //  中断
        mcause &= ~(1UL << 63);
        switch (mcause) {
            case Machine_Software_Interrupt:
                //  这里留一个例子
                break;
                
            default:
                break;
        }
        return regs;
    }
    
    switch (mcause) {
        case Environment_Call_From_S_mode:
        case Environment_Call_From_M_mode:
            sbi_ecall_handler(regs);
            regs->mepc += 4;
            break;
            
        default:
            break;
    }
    
    return regs;
}

