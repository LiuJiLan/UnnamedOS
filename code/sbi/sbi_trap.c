//
//  sbi_trap.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/4/19.
//

#include "sbi_trap.h"
#include "sbi_riscv.h"
#include "clint.h"

void MEIP_FAULT(char * s) {
}

void M_MODE_Exception(int mcause) {
}

//  BUG!!!!
//  引用了我自己写的程序, 但是hart_mask是将每个位上的hart都启动
//  我们这个写法只能设置一个

//  目标hart的计算方法
//  (设置"hart_mask can be ignored and all
//  available harts must be considered"的情况除外)
//  目标hart = hart_mask的位(最低位是0位) + hart_mask_base
//  对于hart_mask, 例如, 0xb1是0, 0b10是1
//  对于hart_mask_base, 例如, 0是0, 1是1, 2是2
int count_hart_mask(unsigned long hart_mask) {
    int count = -1;
    while (hart_mask) {
        hart_mask = hart_mask >> 1;
        count++;
    }
    return count;
}


struct sbi_trap_regs * sbi_trap_handler(struct sbi_trap_regs * regs) {
    regs_t mcause = regs->mcause;
    int hart_id = (int)r_tp();
    
    if (mcause & (0x1UL << 63)) {   //  中断
        mcause &= ~(1UL << 63);
        switch (mcause) {
            case Machine_Software_Interrupt:
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
                
            case Machine_External_Interrupt:
                //理论上我们的流程也是清pending+转发
                //但是SEIP无法被设置
                //实践中其实没有下陷到M态的情况(大概)
                MEIP_FAULT("MEIP in M mode.");
                break;
                
            default:
                break;
        }
        return regs;
    }
    
    switch (mcause) {
        case Environment_Call_From_S_mode:
        case Environment_Call_From_M_mode:
            //  只实现Timer扩展和IPI扩展
            if (regs->a7 == 0x54494D45 && regs->a6 == 0) {
                //  只清除mip.STIP, MTIP已经trap中处理了
                asm volatile("li    t0, 0x1 << 5;"
                             "csrc  mip, t0;"
                             ://    无输出
                             ://    无输入
                             :"t0");
                set_CLINT_timer_interval(regs->a0);
                regs->a0 = 0;
            } else if (regs->a7 == 0x735049 && regs->a6 == 0) {
                int target_hart_id = count_hart_mask(regs->a0);
                target_hart_id += (int)regs->a1;
                set_CLINT_MSIP(target_hart_id);
                regs->a0 = 0;
            } else {
                regs->a0 = -2;  //SBI_ERR_NOT_SUPPORTED
            }
            
            regs->mepc += 4;
            break;
            
        default:
            M_MODE_Exception((int)regs->mcause);
            break;
    }
    
    return regs;
}
