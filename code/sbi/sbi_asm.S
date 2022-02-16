//
//  sbi_asm.S
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/21.
//

//  基本沿用bootasm.S

#include "sbi_param.h"

.text
.global start
start:

#define DEBUG
#ifdef DEBUG
li    x5 , 5
li    x6 , 6
li    x7 , 7
li    x8 , 8
li    x9 , 9
//  a0和a1用来传导参数
//li    x10 , 10
//li    x11 , 11
li    x12 , 12
li    x13 , 13
li    x14 , 14
li    x15 , 15
li    x16 , 16
li    x17 , 17
li    x18 , 18
li    x19 , 19
li    x20 , 20
li    x21 , 21
li    x22 , 22
li    x23 , 23
li    x24 , 24
li    x25 , 25
li    x26 , 26
li    x27 , 27
li    x28 , 28
li    x29 , 29
li    x30 , 30
li    x31 , 31
#endif
    //  一般会传入些值, a0是hart id, a1是dtb的地址
    //  我们在此处重新读一次hart id到t0
    csrr    a0, mhartid // 获取hart id
    mv      tp, a0
    mv      s1, a1
    //  我们转存的位置模仿于u-boot
    //  gp和tp在资料中说的作用是编译器松弛,
    //  暂不清楚随意设置tp会不会造成某些后果
    //  (u-boot随后做了设置, 我们在用完后打算直接清零)
    //  a1存s1而不是s0主要是考虑到s0同时兼顾fp的作用(暂时不知道干嘛的)
    //  而s1又是ABI中随后一个由callee保存的寄存器(C语言会帮我们保护)

    csrw    mie, zero   // 关中断

    la      t0, trap_vector
    csrw    mtvec, t0

    //  设置每一个hart的栈
    addi    t0, tp, 0x1
    li      t1, SBI_STACK_SIZE
    mul     t1, t0, t1      //  如果没有M指令集可以改用srl
    la      t0, sbi_end

    add     t0, t0, t1
    mv      sp, t0          //  这里不管mscratch下面的临时空间,
                            //  因为如果在sbi转跳之前, 还是会继续使用sp
                            //  如果sbi转跳之后, 则不用担心(sbi里面的函数已经执行完了)
    csrw    mscratch, t0
    
    bnez    tp, not_start_hart
    
    mv      a0, tp
    mv      tp, zero        //  之前说的, 清空tp, 等到我搞懂它干什么用的再说

    mv      a1, s1
    //  准备传参
    
    call    sbimain

spin:   //  这个spin是给sbimain失败时用的, 当然不应该发生这种事
    j       spin

not_start_hart:
    mv      a0, tp          //  存a0(其实细想不用存)
    
    //  开软中断
    li      t0, 0x1 << 3
    csrs    mie, t0
    
    j       wfi_spin

wfi_spin:
    wfi
    //nop
    csrr    t0, mip
    li      t1, 0x1 << 3    //  检查是否是ipi

    and     t0, t0, t1
    bnez    t0, test
    j       wfi_spin

test:
    csrc    mip, t1     //  清除软中断
    call    other
