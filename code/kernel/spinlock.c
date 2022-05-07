//
//  spinlock.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/4/25.
//

#include "spinlock.h"
#include "riscv.h"
#include "hart.h"

void initlock(struct spinlock * lk, char * name) {
    lk->name = name;
    lk->locked = 0;
}

void push_intr(void) {
    regs_t sstatus = r_sstatus();
    int old = sstatus & 0x2U;
    sstatus &= ~0x2U;
    w_sstatus(sstatus);
    //  一定要先关中断, 因为要调用my_hart
    
    struct hart * mine = my_hart();
    if (mine->n_intr == 0) {
        mine->old_intr = old;
    }
    mine->n_intr += 1;
}

void pop_intr(void) {
    struct hart * mine = my_hart();
    if (--mine->n_intr == 0 && mine->old_intr) {
        regs_t sstatus = r_sstatus();
        sstatus |= 0x2U;
        w_sstatus(sstatus);
    }
}


void acquire(struct spinlock * lk) {
    //  与xv6不同我们不再做是否自己持有这个锁还要上锁的检查
    push_intr();
    
    while (__sync_lock_test_and_set(&lk->locked, 1) != 0);
    __sync_synchronize();
}

void release(struct spinlock * lk) {
    __sync_synchronize();
    __sync_lock_release(&lk->locked);
    
    pop_intr();
}
