//
//  sbi_trap.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/27.
//

#include "sbi_trap.h"
#include "sbi_type.h"
#include "sbi_ecall.h"

#include "sbi_defs.h"
#include "clint.h"

struct sbi_trap_regs * sbi_trap_handler(struct sbi_trap_regs * regs) {
    regs_t mcause = regs->mcause;
    int hart_id = 0;
    
    if (mcause & (0x1UL << 63)) {   //  中断
        mcause &= ~(1UL << 63);
        switch (mcause) {
            case Machine_Software_Interrupt:
                hart_id = get_mhartid();
                //  清pending
                clear_CLINT_MSIP(hart_id);
                //  转发SSIP
                asm volatile("li    t0, 0x2;"
                             "csrs  mip, t0;"
                             :
                             :
                             :"t0");
                break;
                
            case Machine_Timer_Interrupt:
                //  清pending
                set_CLINT_mtimecmp_infinitely();
                //  转发STIP
                asm volatile("li    t0, 0x20;"
                             "csrs  mip, t0;"
                             :
                             :
                             :"t0");
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

