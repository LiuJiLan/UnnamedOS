//
//  entry.S
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/4/22.
//

#include "param.h"
#include "memlayout.h"


.text

.global _start
_start = V2P_WO(entry)

.global for_gdb_physical
for_gdb_physical = V2P_WO(for_gdb_virtual)

.global entry
entry:
    csrw    sie, zero
    csrci   sie, 0x1 << 1   //  关SIE
    li      t0, 0x0222
    csrs    sie, t0

    //  由于内核空间是紧密排列的
    //  所以我们要将栈有个数的栈空间紧密的贴合在内核的.data中
    //  我们的M态空间的栈由于为了兼容比赛而选择没有紧密贴合,
    //  M态的栈是贴合在M态空间的最高地址处(其实也有hart数量的限制,
    //  但由于M态空间有2MiB所以没有很详细的处理)
    li      t0, N_HART
    bge     tp, t0, spin
    
    //  注意la加载的永远是pc相关,
    //  所以需要其他方式加载进寄存器
    //  所以所有使用虚拟地址的值都需要特殊方法加载
    
    //  加载栈指针与sscratch
    lui     t0, %hi(stack_top)
    addi    t0, t0, %lo(stack_top)

    li      t1, KERNEL_STACK_SIZE
    mul     t1, tp, t1
    sub     sp, t0, t1
    
    csrw    sscratch, sp
    
    //  开启SUM位, 让内核可以访问用户态
    //  有无必要开启这个功能有待商榷
    //  因为本质上内核回收了所有的物理内存,
    //  也就是说有一个内核权限和用户权限同时指向了同一个物理地址
    //  所以本质上我们不需要开启这个位
    csrr    t0, sstatus
    li      t1, 0x1 << 18   //  SUM
    or      t0, t0, t1
    csrw    sstatus, t0

    //  加载stvec
    lui     t0, %hi(trap_vector)
    addi    t0, t0, %lo(trap_vector)
    csrw    stvec, t0
    
    //  加载satp
    //  注意, satp应该使用的是物理地址,
    //  但是为了避免意外错误还是先加载虚拟地址再计算出物理地址
    lui     t0, %hi(tmp_ker_pt)
    addi    t0, t0, %lo(tmp_ker_pt)

    li      t1, V_P_DIFF
    sub     t0, t0, t1

    srli    t0, t0, 12      //  右移, 空位填0

    li      t1, 0x8 << 60   //  8代表Sv39分页模式
    or      t0, t0, t1

    csrw    satp, t0        //  启动分页
    sfence.vma              //  刷新快表

for_gdb_virtual:
    //  用于调试

    
    //  由于sp用的是虚拟地址,
    //  所以我们就不再用C语言设置S态的CSR了
    lui     t0, %hi(kmain)
    addi    t0, t0, %lo(kmain)
    jr      t0


spin:
    wfi
    j   spin


.data
stack_end:
    .zero KERNEL_STACK_SIZE * N_HART
stack_top:

.align 12   //  4096 bytes 对齐
tmp_ker_pt: //  temp kernel page table
    //  0x8000_0000 -> 0x8000_0000 (2 GiB)
    //  0x8000_0000 -> 0xFFFF_FFFF_C000_0000 (2 GiB)
    .zero 2 * 8                     // [0][1]
    .8byte (0x80000 << 10) | 0xcf   // [2]
    .zero 508 * 8                   // [3]~[510]
    .8byte (0x80000 << 10) | 0xcf   // [511]

    //  注意使用的是大页面, 本身就是临时页表, 能多小就多小
