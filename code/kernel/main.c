//
//  main.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/12.
//

#include "defs.h"
#include "memlayout.h"
#include "mmu.h"
#include "sbi.h"

void testfunc(regs_t a1, regs_t a2) {
    
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

extern char end[];   //  链接脚本中提供, 更多详情见README/main.c/引入外部symbol

int main(void) {
    kinit1((void *)end, (void *)P2V(0x80200000 + 0x200000));    // 见README
    panic("kinit1");
    kvmalloc();
    panic("SBI");
    
    struct sbiret test;
    
    sbi_set_timer(5000000);
    while (1) {
        sbi_console_putchar('.');
        //panic(".");
        if (have_sip_stip()) {
            sbi_console_putchar('C');
            //panic("GOOD!");
            break;
        }
    }
    
    //  由于STIP没有被清除, 所以只会打印一个加号就会打印X
    while (1) {
        sbi_console_putchar('+');
        if (have_sip_stip()) {
            sbi_console_putchar('X');
            break;
        }
    }
    
    sbi_set_timer(50000);
    //  重设后会清除STIP
    while (1) {
        sbi_console_putchar('-');
        if (have_sip_stip()) {
            sbi_console_putchar('Y');
            break;
        }
    }
    
    panic("END!");
    
    while (1) {
    }
}

pte_t entrypgdir[];

__attribute__((__aligned__(PGSIZE)))
pte_t entrypgdir[NPTE1] = {
    //  注意页表项的高位新增了一些用于新增指令集的位, 我们暂时设0
    [2] = (0x80000 << 10) | 0xcf,
    //  我们没有像xv6一样使用mmu.h中的宏, 因为只是临时页表
    [511] = (0x80000 << 10) | 0xcf
};
