//
//  entry.S
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/9.
//

#include "memlayout.h"
//#include "mmu.h"
//  xv6中"mmu.h"存储了部分有关建立页表所需的宏定义
//  例如分页模式之类的参数
//  第二版再来讨论是否需要把这些东西放入头文件中
#include "param.h"

//  _start是给bootblock使用用来加载elf文件的程序入口
//  entry使用的是链接脚本中的虚拟地址
//  所以链接脚本中入口被设为_start, 而后续的摆放则是由虚拟地址开始摆放

.text

.global _start
_start = V2P_WO(entry)

//  V2P_WO宏定义在memlayout.h
//  memlayout.h中的部分变量是暂定的, 并且要和链接脚本相呼应

.global for_gdb_physical
for_gdb_physical = V2P_WO(for_gdb_virtual)
//  自动调试用的, 见for_gdb_virtual

.global entry
entry:
    //  xv6中此处只开启了大页模式, 推测是出于编程简单的原因
    //  在此也暂时保持和xv6保持一致, 暂时只采用一级页表映射

    //  设置页表
    //  开始设置satp寄存器

    lui     t0, %hi(entrypgdir)
    addi    t0, t0, %lo(entrypgdir)
    //  注意这个方法加载的其实是符号位扩展的32位地址
    li      t1, V_P_DIFF
    sub     t0, t0, t1

    srli    t0, t0, 12      //  右移, 空位填0

    li      t1, 0x8 << 60   //  8代表Sv39分页模式
    or      t0, t0, t1

    csrw    satp, t0        //  启动分页
    sfence.vma              //  刷新快表

for_gdb_virtual:
    //  由于在没开页表时使用gdb自动设置一些用虚拟地址的symbol会失败
    //  这里专门给一个点用于调试的到这个点之后就能用虚拟地址的symbol了
    
    //  la      sp, stack + KSTACKSIZE  //  "param.h"中指定
    lui     sp, %hi(stack + KSTACKSIZE)
    addi    sp, sp, %lo(stack + KSTACKSIZE)

    //  la      t0, main        //  进kernel的main函数
    lui     t0, %hi(kmain)
    addi    t0, t0, %lo(kmain)
    jr      t0

.comm stack, KSTACKSIZE
