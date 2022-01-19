//
//  memlayout.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/9.
//

//  EXTMEM是xv6中内核所在的物理地址,
//  同时也是xv6中所使用平台的实际内存的映射地址,
//  相当于SRAM映射到内存的那个起始地址0x80000000U

//  KERNBASE是xv6中内核在虚拟内存中的起始位置
//  我当时很好奇为什么需要再多加一个KERNLINK的内核链接地址
//  原因:
//  xv6中把实际物理地址0x0-0x100000中的base memory和I/O space也当做了内核的一部分
//  这部分和真正的“内核代码”一起被映射到了KERNBASE
//  所以KERNLINK是xv6中“内核代码”运行的地方

//  我的虚拟内存映射参考了非教学版 rCore 的虚拟地址和物理地址的映射关系
//  https://rcore-os.github.io/rCore-Tutorial-deploy/docs/lab-3/guide/part-1.html
//  在我的代码的意义里, KERNBASE和KERNLINK都是同一个值, 都是0xFFFF_FFFF_C020_0000

#ifndef memlayout_h
#define memlayout_h

//#define EXTMEM  0x100000            // Start of extended memory
//#define PHYSTOP 0xE000000           // Top physical memory
//#define DEVSPACE 0xFE000000         // Other devices are at high addresses

//#define KERNBASE 0x80000000         // First kernel virtual address
//#define KERNLINK (KERNBASE+EXTMEM)  // Address where kernel is linked

//  命名暂时与xv6保持一致, 之后再换名字
#define EXTMEM  0x80000000U
//  rCore把它放在了0x80200000
//  巧合的是我们内核所在的物理地址跟xv6中的虚拟地址竟是一个值, 时代在进步
#define PHYSTOP 0x80600000U
//  xv6中没有做内存探测, 内存的上限是通过这个值指定的
//  我们在二周目可以用dtb探测出的内存值给他给予设定
//  由于k210的一些设定, 我们还是将内存大小设为6M

//  #define KERNBASE 0xFFFFFFFFC0200000U    //  参考rCore
//  暂改为0xFFFFFFFFC0000000U, 原因见entry.S大页有关部分
#define KERNBASE 0xFFFFFFFFC0000000U
#define KERNLINK KERNBASE               //  注意链接脚本要和这里的值一致

//  #define V_P_DIFF 0xFFFFFFFF40200000U
#define V_P_DIFF 0xFFFFFFFF40000000U    //  由于KERNBASE的改变我们也暂改
//  xv6中由于设计上的巧合导致虚拟内存和物理内存的差值正好是KERNBASE
//  我们这里额外定义一个虚拟内存和物理内存的差值, 通过KERNBASE-EXTMEM得出

#define V2P(a) (((unsigned long) (a)) - V_P_DIFF)   //  64位下改了数据类型
#define P2V(a) ((void *)(((unsigned char *) (a)) + V_P_DIFF))

//  纯数值的转换
#define V2P_WO(x) ((x) - V_P_DIFF)    // same as V2P, but without casts //  不带类型转换
#define P2V_WO(x) ((x) + V_P_DIFF)    // same as P2V, but without casts

//  IO部分参考qemu
//  https://github.com/qemu/qemu/blob/master/hw/riscv/virt.c

//  如果是k210参考其SDK的platform.h

#endif /* memlayout_h */
