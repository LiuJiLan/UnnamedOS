//
//  mmu.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/12.
//

#ifndef mmu_h
#define mmu_h

//  页表常量
//  对应xv6中NPDENTRIES、NPTENTRIES、PGSIZE
#define NPTE1       512     //  Sv39下每级表有2^9项
#define NPTE2       512
#define NPTE3       512
//#define PGSIZE      4096

//  页表项flag     //  这个写法太不优雅了, 之后考虑用位操作结构体
#define PTE_V           0x001   // 是否合法
#define PTE_R           0x002   // 读
#define PTE_W           0x004   // 写
#define PTE_X           0x008   // 执行
#define PTE_U           0x010   // 为1表示用户态运行的程序可以通过该页表项完成地址映射
#define PTE_G           0x020   // 资料有写
#define PTE_A           0x040   // Accessed
#define PTE_D           0x080   // Dirty



#endif /* mmu_h */
