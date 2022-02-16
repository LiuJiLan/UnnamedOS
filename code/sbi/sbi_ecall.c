//
//  sbi_ecall.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/31.
//

#include "sbi_ecall.h"

#include "sbi_ecall_base.h"
#include "sbi_ecall_legacy.h"


int sbi_ecall_handler(struct sbi_trap_regs * regs) {
    long * sbiret_error = (void*)(&regs->a0);
    long * sbiret_value = (void*)(&regs->a1);
    //  不使用sbiret结构体, 直接引用指针, 减少开销
    regs_t EID = regs->a7;
    regs_t FID = regs->a6;
    int ret = 0;
    
    //  openSBI使用的是函数指针来处理这一切, 我们暂时不做这么复杂
    switch (EID) {
        case Set_Timer:
        case Console_Putchar:
        case Console_Getchar:
        case Clear_IPI:
        case Send_IPI:
        case Remote_FENCE_I:
        case Remote_SFENCE_VMA:
        case Remote_SFENCE_VMA_with_ASID:
        case System_Shutdown:
            ret = sbi_ecall_legacy_handler(EID, sbiret_error, sbiret_value, regs);
            break;
            
        case Base_Extension:
            ret = sbi_ecall_base_handler(FID, sbiret_error, sbiret_value, regs);
            break;
            
        case Timer_Extension:
        case IPI_Extension:
        case RFENCE_Extension:
        case Hart_State_Management_Extension:
        case System_Reset_Extension:
        case Performance_Monitoring_Unit_Extension:
            *sbiret_error = SBI_ERR_NOT_SUPPORTED;
            break;
            
        default:
            break;
    }
    
    return ret;
}
