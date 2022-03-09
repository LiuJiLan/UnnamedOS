//
//  plic.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/2/28.
//

#include "type.h"
#include "defs.h"

//抄的RVOS, platform.h部分
//注意我们使用的是虚拟地址

#define PLIC_BASE 0xffffffff4c000000
#define PLIC_PRIORITY(id) (PLIC_BASE + (id) * 4)
#define PLIC_PENDING(id) (PLIC_BASE + 0x1000 + ((id) / 32) * 4)
#define PLIC_MENABLE(hart) (PLIC_BASE + 0x2000 + (hart) * 0x80)
#define PLIC_MTHRESHOLD(hart) (PLIC_BASE + 0x200000 + (hart) * 0x1000)
#define PLIC_MCLAIM(hart) (PLIC_BASE + 0x200004 + (hart) * 0x1000)
#define PLIC_MCOMPLETE(hart) (PLIC_BASE + 0x200004 + (hart) * 0x1000)

void plicinit(void) {
    int hart = 0;//  准备重头写起了, 只是让它可以编译
    *(unsigned int *)PLIC_MTHRESHOLD(hart) = 0;//允许所有等级中断
}
