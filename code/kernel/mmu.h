//
//  mmu.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/12.
//

#ifndef mmu_h
#define mmu_h

#include "type.h"

//xv6在type.h还申明了一个pde_t, 由于risc-v定义的页表含义, 我们称任意级的表项都为pte
//我们考虑是否要更改这一称呼

//  页表常量
//  对应xv6中NPDENTRIES、NPTENTRIES、PGSIZE
#define NPTE1       512     //  Sv39下每级表有2^9项
#define NPTE2       512
#define NPTE3       512
#define PGSIZE      4096

#define PGROUNDUP(sz)  (((sz)+PGSIZE-1) & ~(PGSIZE-1))
#define PGROUNDDOWN(a) (((a)) & ~(PGSIZE-1))
//  注意, PGSIZE必须是2的幂(当然肯定是2的幂)

//  页表项flag     //  这个写法太不优雅了, 之后考虑用位操作结构体
#define PTE_V           0x001   // 是否合法
#define PTE_R           0x002   // 读
#define PTE_W           0x004   // 写
#define PTE_X           0x008   // 执行
#define PTE_U           0x010   // 为1表示用户态运行的程序可以通过该页表项完成地址映射
#define PTE_G           0x020   // 资料有写
#define PTE_A           0x040   // Accessed
#define PTE_D           0x080   // Dirty

//  PDX、PTX、PTXSHIFT、PDXSHIFT 会改为这些宏
#define VPN_L2_SHIFT    30
#define VPN_L1_SHIFT    21
#define VPN_L0_SHIFT    12

#define PT_L2_SHIFT(va) (((unsigned long)(va) >> VPN_L2_SHIFT) & 0x1FF)
#define PT_L1_SHIFT(va) (((unsigned long)(va) >> VPN_L1_SHIFT) & 0x1FF)
#define PT_L0_SHIFT(va) (((unsigned long)(va) >> VPN_L0_SHIFT) & 0x1FF)

#define PTE_ADDR(pte)   (((unsigned long)(pte) & ~0x3FF) << 2)
// xv6不用移位的原因是flag位正好是12位
#define PTE_FLAGS(pte)  ((unsigned long)(pte) &  0x3FF)

#endif /* mmu_h */
