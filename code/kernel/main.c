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

testfunc(regs_t a1, regs_t a2) {
    
}

extern char end[];   //  链接脚本中提供, 更多详情见README/main.c/引入外部symbol

int main(void) {
    kinit1((void *)end, (void *)P2V(0x80200000 + 0x200000));    // 见README
    panic("kinit1");
    kvmalloc();
    panic("SBI");
    
    struct sbiret test;
    
    //void sbi_set_timer(uint64_t stime_value);
    sbi_console_putchar('X');
    panic("putchar");
    //test.value = sbi_console_getchar();
    //sbi_console_putchar(test.value);
    unsigned long hart_mask_num = 0b10;
    sbi_send_ipi(V2P(&hart_mask_num));
    //  临时改成传值, 用于排除物理地址的错误
    int count = 300;
    while (count--) {
        //
    }
    panic("ipi");

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
