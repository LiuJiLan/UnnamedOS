//
//  spinlock.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/2/22.
//

#include "defs.h"
#include "spinlock.h"
#include "riscv.h"


void initlock(struct spinlock *lk, char *name) {
    lk->name = name;
    lk->locked = 0;
    lk->cpu = 0;
}

void acquire(struct spinlock *lk) {
    pushcli(); // disable interrupts to avoid deadlock.
    if(holding(lk))
        panic("acquire");
    
    // riscv指令, 原子交换
    while(__sync_lock_test_and_set(&lk->locked, 1) != 0)
        ;
    
    __sync_synchronize();
    
    lk->cpu = mycpu();
    //getcallerpcs(&lk, lk->pcs);
}

void release(struct spinlock *lk) {
    if(!holding(lk))
        panic("release");
    
    //lk->pcs[0] = 0;
    lk->cpu = 0;
    
    __sync_synchronize();
    
    //  直接用了编译器的
    //  而不是像xv6那样用的汇编
    //  之后详细处理时应该考虑换成自己的代码
    __sync_lock_release(&lk->locked);
    
    popcli();
}

int holding(struct spinlock *lk) {
    int r;
    r = (lk->locked && lk->cpu == mycpu());
    return r;
}

//  注意引发panic的情况是
//  1.是本cpu持有这个锁还要再次上锁
//  2.不是本cpu持有这个锁却要释放这个锁

void pushcli(void) {
    regs_t sstatus;
    sstatus = get_sstatus();
    
    clear_sstatus_SIE();
    if (mycpu()->ncli == 0) {
        mycpu()->intena = sstatus & 0x2U;
        //日后应该设置一个宏而不是直接用0x2
    }
    mycpu()->ncli += 1;
}

void popcli(void) {
    if (get_sstatus() & 0x2U) {
        panic("popcli - interruptible");
    }
    if (--mycpu()->ncli < 0) {
        panic("popcli");
    }
    if (mycpu()->ncli == 0 && mycpu()->intena) {
        set_sstatus_SIE();
    }
}
