//
//  sbi_spinlock.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/2/22.
//

#ifndef sbi_spinlock_h
#define sbi_spinlock_h

#include "cpu.h"

struct spinlock {
    unsigned int locked;
    // Is the lock held?
    
    char *name;
    int hart_id;
};



//  注意由于我们将struct cpu改为了hart_id
//  后续加锁解锁的时候恢复的初始值改为-1

#endif /* sbi_spinlock_h */
