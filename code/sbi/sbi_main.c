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

void test_ipi(void) {
    //*((volatile uint32 *)(0x2000004U)) = 0x1U;
}

int have_sip_stip(void) {
    regs_t sip = 0;
    asm volatile("csrr  %0, sip;"
                 :"=r"(sip)
                 :
                 :);
    if (sip & 0x20U) {
        return  1;
    } else {
        return 0;
    }
}

int have_mip_mtip(void) {
    regs_t mip = 0;
    asm volatile("csrr  %0, mip;"
                 :"=r"(mip)
                 :
                 :);
    if (mip & 0x80U) {
        return  1;
    } else {
        return 0;
    }
}

int have_mip_mtip_clear_and_test(void) {
    regs_t mip = 0;
    asm volatile("li    t0, 0x80;"
                 "csrc  mip, t0;"
                 "csrr  %0, mip;"
                 :"=r"(mip)
                 :
                 :"t0");
    if (mip & 0x80U) {
        return  1;
    } else {
        return 0;
    }
}

int have_sip_stip_set_and_clear_test(void) {
    regs_t mip = 0;
    asm volatile("li    t0, 0x20;"
                 "csrs  mip, t0;"
                 "csrr  %0, mip;"
                 :"=r"(mip)
                 :
                 :"t0");
    if (mip & 0x20U) {
        asm volatile("li    t0, 0x20;"
                     "csrc  mip, t0;"
                     "csrr  %0, mip;"
                     :"=r"(mip)
                     :
                     :"t0");
        if (mip & 0x20U) {
            return  -1;
        } else {
            return 1;
        }
    } else {
        return 0;
    }
}

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
    
    //    timer_load(50000);
    //    while (1) {
    //        uart_putc('.');
    //        if (have_mip_mtip()) {
    //            panic("GOOD!");
    //            break;
    //        }
    //    }
    //    timer_load(500000000);
    //    if (have_mip_mtip_clear_and_test()) {
    //        panic("\nClear Fail!\n");
    //    } else {
    //        panic("\nClear Good!\n");
    //    }
    //    int test = have_sip_stip_set_and_clear_test();
    //    switch (test) {
    //        case -1:
    //            panic("\nCLEAR ERROR.\n");
    //            break;
    //
    //        case 0:
    //            panic("\nSET ERROR.\n");
    //            break;
    //
    //        case 1:
    //            panic("\nGOOD.\n");
    //            break;
    //
    //        default:
    //            break;
    //    }
    
    //    set_CLINT_timer_interval(50000);
    //    while (1) {
    //            uart_putc('.');
    //            if (have_mip_mtip()) {
    //                panic("\nGOOD!\n");
    //                break;
    //            }
    //   }
    //    set_CLINT_mtimecmp_infinitely();
    //    if (have_mip_mtip()) {
    //        panic("\nNO GOOD!\n");
    //    } else {
    //        panic("\nTEST GOOD!\n");
    //    }
    
    
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

void panic(char * s) {
    while (*s) {
        console_putchar(*s++);
    }
    return;
}

void other(regs_t hart_id, uptr_t dtb_addr) {
    //  用于测试其他的核
    //  注意这样用uart_putc其实没有考虑到调用uart的临界区
    //  之后应该使用锁
    char ch = get_mhartid() + '0';
    panic("\nWake Up is Good!\nThe ");
    console_putchar(ch);
    panic(" is Wake Up.\n");
    while (1) {};
}

regs_t get_mhartid(void) {
    return mhartid;
}
