//
//  sbi_main.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/21.
//

#include "sbi_defs.h"

static inline void jump_to_kernel(uptr_t kernel_entry);

void sbimain(void) {
    uptr_t kernel_entry;
    //    kernel_entry = loader_kernel();
    //    if (kernel_entry == 0) {
    //        panic("kernel entry wrong!");
    //    }
    
    kernel_entry = 0x80200000U;
    jump_to_kernel(kernel_entry);
}

static inline void jump_to_kernel(uptr_t kernel_entry) {
    asm volatile(//  设置PMP
                 "li      t0, -1;"          //  0b111...111
                 "csrw    pmpaddr0, t0;"    //  54-63位是WARL, 所以高位随便写(含义见特权手册)
                 
                 "li      t0, 0x0f;"        //  A=0x01 TOR, X=W=R=1
                 "csrs    pmpcfg0, t0;"
                 
                 //  进S-Mode
                 "li      t0, 0x1 << 11;"   //  mstatus.MPP = 0b01 S Mode
                 "csrs    mstatus, t0;"

                 "mv      t0, %0;"
                 "csrw    mepc, t0;"        //  为什么这么做参考手册中mret指令

                 "mret;"
                 ://    无输出
                 :"r"(kernel_entry)
                 :"t0");
}

void panic(char * s) {
    return;
}
