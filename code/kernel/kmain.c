//
//  kmain.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/4/22.
//

#include "hart.h"
#include "memlayout.h"
#include "kalloc.h"
#include "riscv.h"
#include "vm.h"


void panic(char * s) {  //  用于GDB来debug
}

extern char etext[];
extern char kend[]; //  在ld中定义

void kinit1(void);
void kinit2(void);
void kvm_alloc_and_load(void);

void kmain(void) {
    init_hart();
    
    //  物理内存的初始化和虚拟内存的使用会引发问题
    //  想要使用完整的内核页表必须要有管理物理页面的能力
    //  想要管理所有物理内存有需要有完整的页表
    //  所以只能分布的完成, 例如先回收一部分物理页
    
    kinit1();
    kvm_alloc_and_load();
    vm_2_kpgtbl();
    kinit2();
    
    panic("GOOD");
    
    char * p = (void*)0xFFFFFFFFC0600000U;
    p -= 12;
    *p = 12;
    
    panic("GOOD2");
    
    while (1) {
    }   
}

void kinit1(void) {
    char * pend;
    //  物理内存是否 <= 2GiB (Sv39大页范围)
    if (PHYSTOP <= 0x100000000ULL) {
        pend = (void *)PHYSTOP;
    } else {
        pend = (void *)0x100000000ULL;
    }
    
    char * vstart = kend;
    char * vend = P2V(pend);
    
    char * p;
    p = (char *)PGROUNDUP((unsigned long)vstart);
    while (p + PGSIZE <= vend) {
        kfree(p);
        p += PGSIZE;
    }
}

void kinit2(void) {
    char * pstart;
    if (PHYSTOP <= 0x100000000ULL) {
        return;
    } else {
        pstart = (void *)0x100000000ULL;
    }
    
    char * vstart = P2V(pstart);
    char * vend = P2V(PHYSTOP);
    
    char * p;
    p = (char *)PGROUNDUP((unsigned long)vstart);
    while (p + PGSIZE <= vend) {
        kfree(p);
        p += PGSIZE;
    }
}

void kvm_alloc_and_load(void) {
    uptr_t p_etext = (uptr_t)(V2P(etext));
    kvminit();
    kvmmap(0x80200000, p_etext - 0x80200000, PTE_R | PTE_X);
    kvmmap(p_etext, PHYSTOP - p_etext, PTE_R | PTE_W);
}