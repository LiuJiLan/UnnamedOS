//
//  clint.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/4/19.
//

#include "clint.h"
#include "sbi_riscv.h"

#define CLINT_BASE 0x2000000UL
#define CLINT_MSIP(hartid) (CLINT_BASE + 4 * (hartid))
#define CLINT_MTIMECMP(hartid) (CLINT_BASE + 0x4000 + 8 * (hartid))
#define CLINT_MTIME (CLINT_BASE + 0xBFF8) // cycles since boot.

void set_CLINT_MSIP(int hart_id) {
    *(volatile uint32*)CLINT_MSIP(hart_id) = 1;
}

void clear_CLINT_MSIP(int hart_id) {
    *(volatile uint32*)CLINT_MSIP(hart_id) = 0;
}

void set_CLINT_timer_interval(uint64 interval) {
    regs_t id = r_tp();
    
    *(volatile uint64*)CLINT_MTIMECMP(id) = *(volatile uint64*)CLINT_MTIME + interval;
}

void set_CLINT_mtimecmp_infinitely(void) {
    //  我之前能将mtimecmp设置为-1也就是0xFFF...FFF来清除MTIP
    //  但是这次发现这样反而会产生MTIP
    //  经过实验发现能给出尽量大、又好设置的值是0x0FFF...FFF
    
    regs_t id = r_tp();
    regs_t num = -1;
    num = num >> 4;

    *(volatile uint64*)CLINT_MTIMECMP(id) = num;
}
