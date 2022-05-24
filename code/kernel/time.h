//
//  time.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/5/19.
//

#ifndef time_h
#define time_h

#include "time.h"
#include "types.h"
#include "spinlock.h"

struct {
    volatile uint64 nsec;
    volatile uint64 sec;
    struct spinlock lock;
} ktime;
//  不用上锁, 因为只有hart0会更改这个设置
//  BUG!!!
//  其实应该上锁, 因为本质上数据的值需要计算步骤, 会导致别的CPU读取到不正确的值

void time_init(void);
extern void (* time_tick)(void);
uint64 time_get_nsec(void);
uint64 time_get_sec(void);

void time_ring_clock(void);

#endif /* time_h */
