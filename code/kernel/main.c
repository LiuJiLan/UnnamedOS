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

static regs_t mhartid;

regs_t get_mhartid(void) {
    return mhartid;
}

int main(regs_t hart_id, uptr_t dtb_addr) {
    kinit1((void *)end, (void *)P2V(0x80200000 + 0x200000));    // 见README
    panic("kinit1");
    kvmalloc();
    panic("SBI");
    mpinit();
    //cprintf();
    //consoleinit();
    //    uartinit();
    //    pinit();
    //    tvinit();
    //    binit();
    //    fileinit();
    //    ideinit();
    //    startothers();
    //    kinit2(P2V(4*1024*1024), P2V(PHYSTOP));
    //    userinit(); 
    //    mpmain();
    unsigned long hart = 0b10;
    sbi_send_ipi(V2P(&hart));
    
    panic("ipi");
    int count = 3000;
    while (count--) {
        
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
