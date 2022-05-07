//
//  hart.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/4/24.
//

#include "hart.h"
#include "riscv.h"

struct hart harts[N_HART];

void init_hart(void) {
    for (struct hart * i = harts; i < harts + N_HART; i++) {
        i->n_intr = 0;
        i->old_intr = 0;
        i->pid = NULL;
    }
}

struct hart * my_hart(void) {
    //  返回本线程在使用的CPU
    //  注意使用时不能开中断
    return harts + r_tp();
}


