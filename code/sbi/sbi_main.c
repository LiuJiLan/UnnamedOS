//
//  sbi_main.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/21.
//

#include "sbi_defs.h"
#include "sbi_console.h"

static inline void jump_to_kernel(uptr_t kernel_entry,\
                                  regs_t hart_id, uptr_t dtb_addr);

void sbimain(regs_t hart_id, uptr_t dtb_addr) {
    uptr_t kernel_entry;
    //    kernel_entry = loader_kernel();
    //    if (kernel_entry == 0) {
    //        panic("kernel entry wrong!");
    //    }
    
    console_init();
    
    kernel_entry = 0x80200000U;
    jump_to_kernel(kernel_entry, hart_id, dtb_addr);
}

static inline void jump_to_kernel(uptr_t kernel_entry, regs_t hart_id, uptr_t dtb_addr) {
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
                 
                 "mv      a0, %1;"
                 "mv      a1, %2;"

                 "mret;"
                 ://    无输出
                 :"r"(kernel_entry), "r"(hart_id), "r"(dtb_addr)
                 :"t0", "a0", "a1");
}

void panic(char * s) {
    return;
}
