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
#include "proc.h"
#include "trap.h"
#include "time.h"
#include "file.h"
#include "ccache.h"
#include "uart.h"


void panic(char * s) {  //  用于GDB来debug
}

extern char etext[];
extern char kend[]; //  在ld中定义

void kinit1(void);
void kinit2(void);
void kvm_alloc_and_load(void);

volatile static int started = 0;

void kmain(void) {
    regs_t hartid = r_tp();
    
    if (hartid == 0) {
        init_hart();
        
        //  物理内存的初始化和虚拟内存的使用会引发问题
        //  想要使用完整的内核页表必须要有管理物理页面的能力
        //  想要管理所有物理内存有需要有完整的页表
        //  所以只能分布的完成, 例如先回收一部分物理页
        
        kinit1();
        kvm_alloc_and_load();
        vm_2_kpgtbl();
        kinit2();
        
        time_init();    //  这个函数要求关中断
        
        file_init();
        kvmmap(UART0_BASE, UART0_SIZE, PTE_R | PTE_W);
        cdev_init();
        procinit();
        panic("HART 0 IS OK");
        
        __sync_synchronize();
        started = 1;
        
        
    } else {
        while (started == 0);
        __sync_synchronize();
        
        vm_2_kpgtbl();
        panic("OTHER HART 0 IS OK");
    }
    
    //  注意只有这个函数不需要被proc_reschedule包装
    //  因为之前已经设置好了页表和hart的myproc为NULL
    intr_on();
    proc_find_runnable_to_run(0);
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
