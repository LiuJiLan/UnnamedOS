//
//  sbi.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/27.
//

#include "sbi.h"

struct sbiret sbi_ecall(unsigned long EID, unsigned long FID) {
    struct sbiret ret;
    asm volatile("mv    a7, %2;"
                 "mv    a6, %3;"
                 "ecall;"
                 "mv    %0, a0;"
                 "mv    %1, a1;"
                 :"=r"(ret.error), "=r"(ret.value)
                 :"r"(EID), "r"(FID)
                 :"t0", "t1");
    return ret;
}

