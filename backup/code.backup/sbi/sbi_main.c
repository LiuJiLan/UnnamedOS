//
//  sbi_main.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/21.
//

#include "sbi_defs.h"
#include "sbi_console.h"
#include "uart.h"

#include "clint.h"  //  for test

static regs_t mhartid;

static inline void jump_to_kernel(uptr_t kernel_entry,\
                                  regs_t hart_id, uptr_t dtb_addr);

void sbimain(regs_t hart_id, uptr_t dtb_addr) {
    
    uptr_t kernel_entry;
    //    kernel_entry = loader_kernel();
    //    if (kernel_entry == 0) {
    //        panic("kernel entry wrong!");
    //    }
    
    mhartid = hart_id;
    
    console_init();
    uart_init();
    
    set_console_putchar(&uart_putc);
    set_console_getchar(&uart_getc);
    
    cprintf("Going to the kernel.\n");
    
    
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
                 
                 // 开mideleg测一下中断
                 // ssoft, stimer, sext -> 0x222
                 "li      t0, 0x222;"
                 "csrs    mideleg, t0;"
                 "li      t0, 0x80;"
                 "csrs    mie, t0;"
                 "csrs    mstatus, t0;"
                 
                 
                 "mv      a0, %1;"
                 "mv      a1, %2;"
                 
                 "mret;"
                 ://    无输出
                 :"r"(kernel_entry), "r"(hart_id), "r"(dtb_addr)
                 :"t0", "a0", "a1");
}

//void panic(char * s) {
//    while (*s) {
//        console_putchar(*s++);
//    }
//    return;
//}

void other(regs_t hart_id, uptr_t dtb_addr) {
    //  用于测试其他的核
    //  注意这样用uart_putc其实没有考虑到调用uart的临界区
    //  之后应该使用锁
    cprintf("\nWake Up is Good!\nThe %d is Wake Up.\n", hart_id);
    while (1) {};
}

regs_t get_mhartid(void) {
    return mhartid;
}
