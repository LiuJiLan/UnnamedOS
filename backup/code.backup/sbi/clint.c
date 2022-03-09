//
//  clint.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/2/17.
//

#include "clint.h"

//  抄袭改写自RVOS中platform.h有关CLINT的部分

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


//  抄袭改写自RVOS中timer.c部分
void set_CLINT_timer_interval(uint64 interval) {
    regs_t id = get_mhartid();
    
    *(volatile uint64*)CLINT_MTIMECMP(id) = *(volatile uint64*)CLINT_MTIME + interval;
}

void set_CLINT_mtimecmp_infinitely(void) {
    regs_t id = get_mhartid();
    
    *(volatile uint64*)CLINT_MTIMECMP(id) = -1;
}
