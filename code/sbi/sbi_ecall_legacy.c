//
//  sbi_ecall_legacy.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/31.
//

#include "sbi_ecall_legacy.h"
#include "sbi_console.h"
#include "clint.h"

void sbi_set_timer(uint64 stime_value); //  暂时没有考虑32位
void sbi_console_putchar(int ch);
int sbi_console_getchar(void);
void sbi_clear_ipi(void);
void sbi_send_ipi(const unsigned long *hart_mask);
void sbi_remote_fence_i(const unsigned long *hart_mask);
void sbi_remote_sfence_vma(const unsigned long *hart_mask,\
                           unsigned long start,\
                           unsigned long size);
void sbi_remote_sfence_vma_asid(const unsigned long *hart_mask,\
                                unsigned long start,\
                                unsigned long size,\
                                unsigned long asid);
void sbi_shutdown(void);

int sbi_ecall_legacy_handler(regs_t EID, long * error, long * value,\
                             struct sbi_trap_regs * regs) {
    switch (EID) {
        case SBI_SET_TIMER:
            sbi_set_timer(regs->a0);
            *error = SBI_SUCCESS;
            break;
            
        case SBI_CONSOLE_PUTCHAR:
            sbi_console_putchar(regs->a0);
            *error = SBI_SUCCESS;
            break;
            
        case SBI_CONSOLE_GETCHAR:
            *value = sbi_console_getchar();
            *error = SBI_SUCCESS;
            break;
            
        case SBI_CLEAR_IPI:
            sbi_clear_ipi();
            *error = SBI_SUCCESS;
            break;
            
        case SBI_SEND_IPI:
            sbi_send_ipi(regs->a0);
            *error = SBI_SUCCESS;
            break;
            
        case SBI_REMOTE_FENCE_I:
        case SBI_REMOTE_SFENCE_VMA:
        case SBI_REMOTE_SFENCE_VMA_ASID:
        case SBI_SHUTDOWN:
            *error = SBI_ERR_NOT_SUPPORTED;
            break;
            
        default:
            break;
    }
    return 0;
}

void sbi_set_timer(uint64 stime_value) {
    //  注意:
    //  我们只清除sip.STIP, mip.MTIP在重设时间后自然会消除
    //  或者会在M态trap中处清理pending
    asm volatile("li    t0, 0x1 << 5;"
                 "csrc  mip, t0;"
                 ://    无输出
                 ://    无输入
                 :"t0");
    //  手册要求:
    //  "This function also clears the
    //  pending timer interrupt bit."
    
    set_CLINT_timer_interval(stime_value);    //  clint.c
}

void sbi_console_putchar(int ch) {
    console_putchar(ch);
}

int sbi_console_getchar(void) {
    return console_getchar();
}

void sbi_clear_ipi(void) {
    //  不用实现了, SBI的文档说的有道理, 本身清除Xsip.Xsip就行
}

void sbi_send_ipi(const unsigned long * hart_mask) {
    int target_hart_id = count_hart_mask(*hart_mask);
    set_CLINT_MSIP(target_hart_id);
}

void sbi_remote_fence_i(const unsigned long *hart_mask) {}
void sbi_remote_sfence_vma(const unsigned long *hart_mask,\
                           unsigned long start,\
                           unsigned long size) {}
void sbi_remote_sfence_vma_asid(const unsigned long *hart_mask,\
                                unsigned long start,\
                                unsigned long size,\
                                unsigned long asid) {}
void sbi_shutdown(void) {}
