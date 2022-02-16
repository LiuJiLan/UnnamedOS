//
//  sbi_ecall_legacy.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/31.
//

#include "sbi_ecall_legacy.h"
#include "sbi_console.h"

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
    *error = SBI_SUCCESS;
    switch (EID) {
        case SBI_SET_TIMER:
            sbi_set_timer(regs->a0);
            break;
            
        case SBI_CONSOLE_PUTCHAR:
            sbi_console_putchar(regs->a0);
            break;
            
        case SBI_CONSOLE_GETCHAR:
            *value = sbi_console_getchar();
            break;
            
        case SBI_CLEAR_IPI:
            sbi_clear_ipi();
            break;
            
        case SBI_SEND_IPI:
            sbi_send_ipi(regs->a0);
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
    
}

void sbi_console_putchar(int ch) {
    console_putchar(ch);
}

int sbi_console_getchar(void) {
    return console_getchar();
}

void sbi_clear_ipi(void) {
    
}

void sbi_send_ipi(const unsigned long *hart_mask) {
    //  Legacy Extensions传递的是hart_mask的指针
    //  而IPI Extension则改为传值
    //  因为M态不使用页表, 可能会产生地址访问错误
    //  但是我暂时不想实现其他的SBI Extensions
    //  在此处规定, Legacy Extensions传递的一律使用物理地址传值
    //  后续改进中将kernel内核中的sbi_send_ipi再改为IPI Extension
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
