//
//  sbi_ecall_legacy.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/31.
//

#ifndef sbi_ecall_legacy_h
#define sbi_ecall_legacy_h

#include "sbi_ecall.h"

int sbi_ecall_legacy_handler(regs_t EID, long * error, long * value);

#endif /* sbi_ecall_legacy_h */
