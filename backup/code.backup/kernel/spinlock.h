//
//  spinlock.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/2/22.
//

#ifndef spinlock_h
#define spinlock_h

struct spinlock {
    unsigned int locked;
    // Is the lock held?
    
    // For debugging:
    char *name;
    struct cpu *cpu;
    //uint pcs[10];
    //  pcs, program counters
    //  用来记录锁住锁当时的PC值并以栈的结构记录
};

#endif /* spinlock_h */
