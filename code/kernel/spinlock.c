//
//  spinlock.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/4/25.
//

#include "spinlock.h"
#include "riscv.h"
#include "hart.h"
#include "intr.h"

void initlock(struct spinlock * lk, char * name) {
    lk->name = name;
    lk->locked = 0;
}

void acquire(struct spinlock * lk) {
    //  与xv6不同我们不再做是否自己持有这个锁还要上锁的检查
    intr_push_off();
    
    while (__sync_lock_test_and_set(&lk->locked, 1) != 0);
    __sync_synchronize();
}

void release(struct spinlock * lk) {
    __sync_synchronize();
    __sync_lock_release(&lk->locked);
    
    intr_pop_off();
}
