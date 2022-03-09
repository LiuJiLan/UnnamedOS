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
