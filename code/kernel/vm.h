//
//  vm.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/4/27.
//

#ifndef vm_h
#define vm_h

#include "types.h"

//  变量说明:
//  k / u: kernel / user
//  p / v: physical / virtual
//  a:     address
//  pgtbl: page table
//  pte:   page table entry
//  perm:  permission (RWX and something else)

//  注意所有pgtbl使用的是内核页表的虚拟地址

//  注意riscv所有的分页都是12 + N * 9的模式
//  所以每个表项必然是512项, 4KiB大

#define PTE_V (1L << 0) // valid
#define PTE_R (1L << 1)
#define PTE_W (1L << 2)
#define PTE_X (1L << 3)
#define PTE_U (1L << 4) // 1 -> user can access

typedef uint64 pte_t;
typedef pte_t* pgtbl_t;

void kvminit(void);
int kvmmap(uptr_t pa, uint64 sz, int perm);
void vm_2_kpgtbl(void);

pgtbl_t vm_init_upgtbl(void);
void vm_delete_upgtbl(pgtbl_t upgtbl);

#endif /* vm_h */
