//
//  sbi_asm.S
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/21.
//

//  基本沿用bootasm.S

#define SBI_STACK_TOP 0x801FFFF0U

.section .text
.globl start
start:
    csrr    a0, mhartid // 获取hart id
    mv      tp, a0
    csrw    mie, zero   // 关中断
    
    bnez    tp, not_start_hart

    li      sp, SBI_STACK_TOP
    
    call    sbimain

spin:
    j       spin

not_start_hart:
    wfi
    //暂未实现, 根据手册wfi后应跟随一定的判断, 等待ipi的唤醒
    j       not_start_hart
