//
//  sbi_spinlock.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/2/22.
//

#include "sbi_spinlock.h"

#include "sbi_defs.h"
#include "sbi_riscv.h"

void initlock(struct spinlock *lk, char *name) {
    lk->name = name;
    lk->locked = 0;
    lk->hart_id = -1;
}

void acquire(struct spinlock *lk) {
    pushcli();
    if(holding(lk))
        panic("acquire");
    
    // riscv指令, 原子交换
    while(__sync_lock_test_and_set(&lk->locked, 1) != 0)
        ;
    
    __sync_synchronize();
    
    lk->hart_id = get_mhartid();
    //getcallerpcs(&lk, lk->pcs);
    
    //  涉及到有关中断的处理的问题, 我们暂时不处理
}

void release(struct spinlock *lk) {
    if(!holding(lk))
        panic("release");
    
    //lk->pcs[0] = 0;
    lk->hart_id = -1;
    
    __sync_synchronize();
    
    //  直接用了编译器的
    //  而不是像xv6那样用的汇编
    //  之后详细处理时应该考虑换成自己的代码
    __sync_lock_release(&lk->locked);
    
    //暂时不处中断的一些事宜
    popcli();
}

int holding(struct spinlock *lk) {
  int r;
  r = (lk->locked && lk->hart_id == get_mhartid());
  return r;
}

void pushcli(void) {
    regs_t mstatus;
    mstatus = get_mstatus();
    
    clear_mstatus_MIE();
    if (mycpu()->ncli == 0) {
        mycpu()->intena = mstatus & 0x8U;
    }
    mycpu()->ncli += 1;
}

void popcli(void) {
    if (get_mstatus() & 0x8U) {
        panic("popcli - interruptible");
    }
    if (--mycpu()->ncli < 0) {
        panic("popcli");
    }
    if (mycpu()->ncli == 0 && mycpu()->intena) {
        set_mstatus_MIE();
    }
}

