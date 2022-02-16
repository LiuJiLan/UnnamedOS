//
//  sbi_ecall_base.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/31.
//

#ifndef sbi_ecall_base_h
#define sbi_ecall_base_h

#include "sbi_ecall.h"

int sbi_ecall_base_handler(regs_t FID, long * error, long * value,\
                           struct sbi_trap_regs * regs);

#define SBI_GET_SBI_SPEC_VERSION    0
#define SBI_GET_SBI_IMPL_ID         1
#define SBI_GET_SBI_IMPL_VERSION    2
#define SBI_PROBE_EXTENSION         3
#define SBI_GET_MVENDORID           4
#define SBI_GET_MARCHID             5
#define SBI_GET_MIMPID              6

#endif /* sbi_ecall_base_h */
