//
//  sleeplock.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/5/22.
//

//  sleeplock本质是对proc_sleep_for_reason和proc_wakeup_for_reason的包装
//  sleeplock包装了一个会被其他CPU(上所拥有的进程)长时间占用的flag
//  这个flag以locked的形式出现, 但其实表达的是BUSY的概念
//  事实上是否真的需要特地包装一个sleeplock出来都是值得商榷的

//  没有像xv6那样维护一个holdingsleep
//  xv6中无论是自旋锁还是sleeplock的hold函数都是用于报警的

//  其实更复杂的逻辑还是适合自旋锁
//  短时的等待也适合自旋锁
//  重复强调:
//  睡眠锁只是对一些用到自旋锁的BUSY行为的包装

#ifndef sleeplock_h
#define sleeplock_h

#include "spinlock.h"
#include "proc.h"

struct sleeplock {
    struct spinlock lk;
    volatile unsigned int locked;
    
    // FOR DEBUG
    char *name;
    pid_t pid;
};

void initsleeplock(struct sleeplock *lk, char *name);
void acquiresleep(struct sleeplock *lk);
void releasesleep(struct sleeplock *lk);

#endif /* sleeplock_h */
