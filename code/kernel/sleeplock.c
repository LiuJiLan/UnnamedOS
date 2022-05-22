//
//  sleeplock.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/5/22.
//

#include "sleeplock.h"
#include "hart.h"

extern void panic(char * s);

void initsleeplock(struct sleeplock *lk, char *name) {
    initlock(&lk->lk, "sleep lock");
    lk->name = name;
    lk->locked = 0;
    lk->pid = 0;
}

void acquiresleep(struct sleeplock *lk) {
    acquire(&lk->lk);
    //  为什么不是if?
    //  因为这个函数还可以在系统没有初始化的时候运行
    //  这个时候locked依靠中断来改变
    while (lk->locked) {
        proc_sleep_for_reason(&lk->lk, lk);
        //  注意, 这个时候已经不会向后执行了
        //  proc_sleep_for_reason中已经转跳了
    }
    lk->locked = 1;
    struct proc * proc = my_proc();
    if (proc) {
        lk->pid = my_proc()->pid;
    } else {
        lk->pid = -1;
    }
    release(&lk->lk);
}

void releasesleep(struct sleeplock *lk) {
    acquire(&lk->lk);
    lk->locked = 0;
    lk->pid = 0;
    proc_wakeup_for_reason(lk);
    release(&lk->lk);
}
