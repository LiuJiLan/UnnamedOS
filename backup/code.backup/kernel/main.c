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

int kmain(regs_t hart_id, uptr_t dtb_addr) {
    kinit1((void *)end, (void *)P2V(0x80200000 + 0x200000));    // 见README
    panic("kinit1");
    kvmalloc();
    panic("SBI");
    mpinit();
    //lapicinit();
    //seginit();    //我们不需要管理段
    //picinit();    //作用是关闭所有中断, 我们的中断本身就是关着的
    //ioapicinit(); //使用plic替换
    
    //我们plic init的作用也跟ioapicinit有着不同
    //我们尽量保持函数结构的相同
    //比如我们也引出了plic init与plic enable两个函数
    //另外我们没有参考了xv6-k210的plicinithart();
    //我们将这个函数也直接放入plic init中
    plicinit();
    
    //consoleinit();//由于需要解决file的问题, 所以先去看file
    //    uartinit();
    pinit();
    //    tvinit();
    //    binit();
    //    fileinit();
    //    ideinit();
    //    startothers();
    //    kinit2(P2V(4*1024*1024), P2V(PHYSTOP));
    //    userinit();
    //    mpmain();
    
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
