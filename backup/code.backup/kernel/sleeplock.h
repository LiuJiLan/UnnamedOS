//
//  sleeplock.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/3/5.
//

#ifndef sleeplock_h
#define sleeplock_h

#include "spinlock.h"   

struct sleeplock {
    unsigned int locked;       // Is the lock held?
    struct spinlock lk; // spinlock protecting this sleep lock
    
    // For debugging:
    char *name;        // Name of lock.
    int pid;           // Process holding lock
};

#endif /* sleeplock_h */
