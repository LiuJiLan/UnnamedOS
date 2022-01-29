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
li    x5 , 5
li    x6 , 6
li    x7 , 7
li    x8 , 8
li    x9 , 9
li    x10 , 10
li    x11 , 11
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

    csrr    a0, mhartid // 获取hart id
    mv      tp, a0
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

    ecall
    
    call    sbimain

spin:
    j       spin

not_start_hart:
    wfi
    //暂未实现, 根据手册wfi后应跟随一定的判断, 等待ipi的唤醒
    j       not_start_hart
