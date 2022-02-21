//
//  clint.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/2/17.
//

#ifndef clint_h
#define clint_h

#include "sbi_type.h"

void set_m_s_sip(int hart_id);
void clear_m_s_sip(int hart_id);

void timer_load(uint64 interval);

#endif /* clint_h */
