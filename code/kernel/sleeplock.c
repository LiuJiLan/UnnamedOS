//
//  sleeplock.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/3/5.
//

#include "sleeplock.h"
#include "defs.h"

void initsleeplock(struct sleeplock *lk, char *name) {
    initlock(&lk->lk, "sleep lock");
    lk->name = name;
    lk->locked = 0;
    lk->pid = 0;
}

void acquiresleep(struct sleeplock *lk) {
    acquire(&lk->lk);
    while (lk->locked) {
        //sleep(lk, &lk->lk);   proc中的函数, 暂未实现
    }
    lk->locked = 1;
    //lk->pid = myproc()->pid;
    release(&lk->lk);
}

void releasesleep(struct sleeplock *lk) {
    acquire(&lk->lk);
    lk->locked = 0;
    lk->pid = 0;
    //wakeup(lk);   //同为proc中的函数, 暂未实现
    release(&lk->lk);
}

int holdingsleep(struct sleeplock *lk) {
    int r;
    
    acquire(&lk->lk);
    //r = lk->locked && (lk->pid == myproc()->pid);
    release(&lk->lk);
    return r;
}



