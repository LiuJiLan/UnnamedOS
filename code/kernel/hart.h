//
//  hart.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/4/24.
//

//  hart主要考虑的是每hart的私有区
//  私有的含义是:每个hart对自己的私有区可读可写
//  对其他hart的私有区(如有必要)只读

#ifndef hart_h
#define hart_h

#include "param.h"

struct hart {
    int n_intr; //  spinlock加锁的层数
    int old_intr;   //  在第一次加锁之前的中断状态
    int * pid; // 本cpu拥有的进程, 注意之后应该改为进程的指针
};

extern struct hart harts[N_HART];

void init_hart(void);
struct hart * my_hart(void);


#endif /* hart_h */
