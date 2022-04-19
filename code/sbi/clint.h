//
//  clint.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/4/19.
//


#include "sbi_types.h"

#ifndef clint_h
#define clint_h

void set_CLINT_MSIP(int hart_id);
void clear_CLINT_MSIP(int hart_id);

void set_CLINT_timer_interval(uint64 interval);
void set_CLINT_mtimecmp_infinitely(void);


#endif /* clint_h */
