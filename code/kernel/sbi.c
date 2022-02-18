//
//  sbi.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/27.
//

#include "sbi.h"

void sbi_ecall(regs_t EID, regs_t FID, long * sbiret_error, long * sbiret_value,
               regs_t arg0, regs_t arg1, regs_t arg2, regs_t arg3) {
    asm volatile("mv    a7, %2;"
                 "mv    a6, %3;"
                 "mv    a0, %4;"
                 "mv    a1, %5;"
                 "mv    a2, %6;"
                 "mv    a3, %7;"
                 "ecall;"
                 "mv    %0, a0;"
                 "mv    %1, a1;"
                 :"=r"(*sbiret_error), "=r"(*sbiret_value)
                 :"r"(EID), "r"(FID), "r"(arg0), "r"(arg1), "r"(arg2), "r"(arg3)
                 :);
}

struct sbiret sbi_get_sbi_spec_version(void) {
    struct sbiret ret;
    sbi_ecall(0x10, 0, &ret.error, &ret.value, 0, 0, 0, 0);
    return ret;
}

struct sbiret sbi_get_sbi_impl_id(void) {
    struct sbiret ret;
    sbi_ecall(0x10, 1, &ret.error, &ret.value, 0, 0, 0, 0);
    return ret;
}

struct sbiret sbi_get_sbi_impl_version(void) {
    struct sbiret ret;
    sbi_ecall(0x10, 2, &ret.error, &ret.value, 0, 0, 0, 0);
    return ret;
}

struct sbiret sbi_probe_extension(regs_t EID) {
    struct sbiret ret;
    sbi_ecall(0x10, 3, &ret.error, &ret.value, EID, 0, 0, 0);
    return ret;
}

struct sbiret sbi_get_mvendorid(void) {
    struct sbiret ret;
    sbi_ecall(0x10, 4, &ret.error, &ret.value, 0, 0, 0, 0);
    return ret;
}

struct sbiret sbi_get_marchid(void) {
    struct sbiret ret;
    sbi_ecall(0x10, 5, &ret.error, &ret.value, 0, 0, 0, 0);
    return ret;
}

struct sbiret sbi_get_mimpid(void) {
    struct sbiret ret;
    sbi_ecall(0x10, 6, &ret.error, &ret.value, 0, 0, 0, 0);
    return ret;
}

//#########################

void sbi_set_timer(uint64 stime_value) {
    struct sbiret ret;
    sbi_ecall(0x00, 0, &ret.error, &ret.value, 0, 0, 0, 0);
}

void sbi_console_putchar(int ch) {
    struct sbiret ret;
    sbi_ecall(0x01, 0, &ret.error, &ret.value, ch, 0, 0, 0);
}

int sbi_console_getchar(void) {
    struct sbiret ret;
    sbi_ecall(0x02, 0, &ret.error, &ret.value, 0, 0, 0, 0);
    return (int)ret.value;
}

void sbi_send_ipi(const unsigned long *hart_mask) {
    struct sbiret ret;
    sbi_ecall(0x04, 0, &ret.error, &ret.value, \
              hart_mask, 0, 0, 0);
}


