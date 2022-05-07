//
//  spinlock.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/4/25.
//

#ifndef spinlock_h
#define spinlock_h

struct spinlock {
    unsigned int locked;
    
    //  for debug
    char * name;
    int hart_id;
    //  单纯的自旋锁并不需要明确知道自己被哪个CPU拥有
};

void initlock(struct spinlock * lk, char * name);
void acquire(struct spinlock * lk);
void release(struct spinlock * lk);

#endif /* spinlock_h */
