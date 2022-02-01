//
//  sbi_ecall_legacy.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/31.
//

#include "sbi_ecall_legacy.h"

int sbi_ecall_legacy_handler(regs_t EID, long * error, long * value) {
    *error = SBI_ERR_NOT_SUPPORTED;
    return 0;
}
