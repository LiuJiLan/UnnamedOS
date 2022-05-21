//
//  intr.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/5/21.
//

#include "intr.h"
#include "riscv.h"
#include "hart.h"

void intr_push_off(void) {
    int old = intr_get();
    
    intr_off();
    if (my_hart()->n_intr == 0) {
        my_hart()->old_intr = old;
    }
    
    my_hart()->n_intr++;
}

void intr_pop_off(void) {
    struct hart * mine = my_hart();
    if (--mine->n_intr == 0 && mine->old_intr) {
        intr_on();
    }
}
