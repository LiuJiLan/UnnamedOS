//
//  sbi_ecall_legacy.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/31.
//

#ifndef sbi_ecall_legacy_h
#define sbi_ecall_legacy_h

#include "sbi_ecall.h"

int sbi_ecall_legacy_handler(regs_t EID, long * error, long * value,\
                             struct sbi_trap_regs * regs);

#define SBI_SET_TIMER               0
#define SBI_CONSOLE_PUTCHAR         1
#define SBI_CONSOLE_GETCHAR         2
#define SBI_CLEAR_IPI               3
#define SBI_SEND_IPI                4
#define SBI_REMOTE_FENCE_I          5
#define SBI_REMOTE_SFENCE_VMA       6
#define SBI_REMOTE_SFENCE_VMA_ASID  7
#define SBI_SHUTDOWN                8

#endif /* sbi_ecall_legacy_h */
