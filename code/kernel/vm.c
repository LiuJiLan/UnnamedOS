//
//  vm.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/18.
//

//  有大量的函数没有实现

#include "defs.h"
#include "memlayout.h"
#include "mmu.h"
#include "riscv.h"

extern char data[]; //  链接脚本提供
pte_t * kpgdir;     //  全局变量

static pte_t * walkpgdir(pte_t *pgdir, const void *va, int alloc)
{
    pte_t *pde_L2;
    pte_t *pgdir_L1;
    pte_t *pde_L1;
    pte_t *pgdir_L0;
    //  这里我本想写一个对应xv6中的变量的关系的, 后来发现比较难写
    //  pgdir_Lx是那一级的表的首地址
    //  pde_Lx是那一级的具体偏移后的地址
    
    //  注意我们的体系下有三层
    
    //    pde = &pgdir[PT_L2_SHIFT(va)];
    //    if(*pde & PTE_V) {
    //        pgtab = (pte_t*)P2V(PTE_ADDR(*pde));
    //    } else {
    //        if(!alloc || (pgtab = (pte_t*)kalloc()) == 0)
    //            return 0;
    //        // Make sure all those PTE_P bits are zero.
    //        memset(pgtab, 0, PGSIZE);
    //        *pde = V2P(pgtab) | PTE_P | PTE_W | PTE_U;
    //    }
    
    pde_L2 = &pgdir[PT_L2_SHIFT(va)];   //  pgdir相当于pgdir_L2
    if(*pde_L2 & PTE_V) {
        pgdir_L1 = (pte_t*)P2V(PTE_ADDR(*pde_L2));
    } else {
        if(!alloc || (pgdir_L1 = (pte_t*)kalloc()) == 0) {
            return 0;
        }
        memset(pgdir_L1, 0, PGSIZE);
        *pde_L2 = (V2P(pgdir_L1) >> 2) | 0xC1;
        //  我们没有设U, 因为还有一个SUM的访问问题
        //  只有XWR都设0才能指向下一级
        
        //  如果上一级都没有被构造那么下一级一定也没有被构造
        //  但是如果pde_L1没有被分配就不能继续向下
    }
    
    pde_L1 = &pgdir_L1[PT_L1_SHIFT(va)];
    if(*pde_L1 & PTE_V) {
        pgdir_L0 = (pte_t*)P2V(PTE_ADDR(*pde_L1));
    } else {
        if(!alloc || (pgdir_L0 = (pte_t*)kalloc()) == 0) {
            return 0;
        }
        memset(pgdir_L0, 0, PGSIZE);
        *pde_L1 = (V2P(pgdir_L0) >> 2) | 0xC1;
    }
    
    return &pgdir_L0[PT_L0_SHIFT(va)];
}

static int mappages(pte_t *pgdir, void *va, uptr_t size,
                    uptr_t pa, int perm) {   //  不明确为何此处使用了static
    char *a, *last;
    pte_t * pte;
    
    a = (char*)PGROUNDDOWN((unsigned long)va);
    last = (char*)PGROUNDDOWN(((unsigned long)va) + size - 1);
    //  注意使用的是size, 然后va可能是不对齐的, 要PGROUNDDOWN
    
    for(;;) {   //  之后改while循环
        if ((pte = (void *)walkpgdir(pgdir, a, 1)) == 0) {
            return -1;
        }
        if (*pte & PTE_V) { //  xv6中使用的是PTE_P, 我们使用Sv39的Valid位
            panic("remap");
        }
        *pte = (pa >> 2)| perm | PTE_V;
        //  能直接这样写*pte = pa | perm | PTE_P;有两个前提条件:
        //  1. x86的页表的flag位正好是12位, 而Sv39却只有10位
        //  2. pa必须是12位对齐的(一般也都满足)
        
        if (a == last) {
            break;
        }
        a += PGSIZE;
        pa += PGSIZE;
    }
    return 0;
}

static struct kmap {            //  内核空间的划分
    void *          virt;       //  虚拟地址
    unsigned long   phys_start; //  物理地址起点
    unsigned long   phys_end;   //  物理地址结束
    int             perm;       //  访问模式(权限)
} kmap[] = {
    //  早期内核比较小的时候内核分成两段会引发remap
    //  PTE_W相当于WRX
    //  { (void*)KERNLINK, V2P(KERNLINK), V2P(data), 0},     // 内核 text+rodata
    //  { (void*)data,     V2P(data),     PHYSTOP,   PTE_W}, // 内核 data+memory
    // { (void*)0xffffffff40000000, 0x0, 0x10000000, PTE_W | PTE_R | PTE_X},    //  这行用于测试
    { (void*)KERNLINK, V2P(KERNLINK), PHYSTOP, PTE_W | PTE_R | PTE_X}
};

pte_t * setupkvm(void) {
    pte_t *pgdir;
    struct kmap *k;
    
    if ((pgdir = (pte_t*)kalloc()) == 0) {
        return 0;
    }
    memset(pgdir, 0, PGSIZE);
    //  我们的内存地址本身就比设备内存高
    //    if (P2V(PHYSTOP) > (void*)DEVSPACE)
    //        panic("PHYSTOP too high");
    for (k = kmap; k < &kmap[NELEM(kmap)]; k++) {
        //&kmap[NELEM(kmap)]这个写法其实是越界了, 以后还是改成指针+偏移量的好
        if (mappages(pgdir, k->virt, k->phys_end - k->phys_start,
                     (uptr_t)k->phys_start, k->perm) < 0) {
            // freevm(pgdir);
            panic("freevm");
            return 0;
        }
    }
    
    return pgdir;
}

void kvmalloc(void) {
    kpgdir = setupkvm();
    switchkvm();
}

void switchkvm(void) {  // 切换到内核页表
    satp(V2P(kpgdir));
}

