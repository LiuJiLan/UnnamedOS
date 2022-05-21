//
//  hart.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/4/24.
//

#include "hart.h"
#include "types.h"
#include "riscv.h"
#include "intr.h"

struct hart harts[N_HART];

void init_hart(void) {
    for (struct hart * i = harts; i < harts + N_HART; i++) {
        i->n_intr = 0;
        i->old_intr = 0;
        i->myproc = NULL;
    }
}

struct hart * my_hart(void) {
    //  返回本线程在使用的CPU
    //  注意使用时不能开中断
    return harts + r_tp();
}

struct proc * my_proc(void) {
    intr_push_off();
    struct hart * mine = my_hart();
    struct proc *p = mine->myproc;
    intr_pop_off();
    return p;
}

void set_myproc(struct proc * p) {
    intr_push_off();
    struct hart * mine = my_hart();
    mine->myproc = p;
    intr_pop_off();
}
