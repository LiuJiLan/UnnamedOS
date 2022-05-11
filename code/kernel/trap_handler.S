//
//  trap_handler.S
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/4/22.
//

//  基本照搬sbi_trap_handler,
//  M态的CSR转成S态的CSR

#define REG_S       sd
#define REG_L       ld
#define REGBYTES    8

#define SSCRATCH_TEMP_NUM 2

#define SBI_TRAP_REGS_SIZE 34
//  除去zero的31个通用寄存器 + sepc + sstatus + scause

.macro  SETUP_SP_AND_SAVE_ORIGINAL_SP
    csrrw   t0, sscratch, t0
    sd      t1, -1*REGBYTES(t0)
    sd      t2, -2*REGBYTES(t0)
    addi    t1, t0, -(SSCRATCH_TEMP_NUM)*REGBYTES
    mv      t2, sp
    
    csrr    sp, sstatus
    srl     sp, sp, 11
    andi    sp, sp, 0b01
    slti    sp, sp, 0b01
    addi    sp, sp, -1
    xor     t2, t2, t1
    and     sp, sp, t2
    xor     t2, t2, t1
    xor     sp, t1, sp

    sd      t2, -(SBI_TRAP_REGS_SIZE - 1)*REGBYTES(sp)
                                //  提前存储原sp
                                //  注意栈顶的位置, 然后sp在1的偏移位置
    ld      t1, -1*REGBYTES(t0)
    ld      t2, -2*REGBYTES(t0)

    csrrw t0, sscratch, t0
.endm

.macro  SAVE_GENERAL_REGS_EXCEPT_SP
    addi    sp, sp, -(SBI_TRAP_REGS_SIZE)*REGBYTES
    REG_S    x1, 0*REGBYTES(sp)
    //REG_S    x2, 1*REGBYTES(sp)   //  sp之前已经存过了
    REG_S    x3, 2*REGBYTES(sp)
    //REG_S    x4, 3*REGBYTES(sp)   //  tp不保存
    REG_S    x5, 4*REGBYTES(sp)
    REG_S    x6, 5*REGBYTES(sp)
    REG_S    x7, 6*REGBYTES(sp)
    REG_S    x8, 7*REGBYTES(sp)
    REG_S    x9, 8*REGBYTES(sp)
    REG_S    x10, 9*REGBYTES(sp)
    REG_S    x11, 10*REGBYTES(sp)
    REG_S    x12, 11*REGBYTES(sp)
    REG_S    x13, 12*REGBYTES(sp)
    REG_S    x14, 13*REGBYTES(sp)
    REG_S    x15, 14*REGBYTES(sp)
    REG_S    x16, 15*REGBYTES(sp)
    REG_S    x17, 16*REGBYTES(sp)
    REG_S    x18, 17*REGBYTES(sp)
    REG_S    x19, 18*REGBYTES(sp)
    REG_S    x20, 19*REGBYTES(sp)
    REG_S    x21, 20*REGBYTES(sp)
    REG_S    x22, 21*REGBYTES(sp)
    REG_S    x23, 22*REGBYTES(sp)
    REG_S    x24, 23*REGBYTES(sp)
    REG_S    x25, 24*REGBYTES(sp)
    REG_S    x26, 25*REGBYTES(sp)
    REG_S    x27, 26*REGBYTES(sp)
    REG_S    x28, 27*REGBYTES(sp)
    REG_S    x29, 28*REGBYTES(sp)
    REG_S    x30, 29*REGBYTES(sp)
    REG_S    x31, 30*REGBYTES(sp)
.endm

.macro  SAVE_SEPC_SSTATUS
    csrr    t0, sepc
    REG_S   t0, 31*REGBYTES(sp)
    csrr    t0, sstatus
    REG_S   t0, 32*REGBYTES(sp)
.endm

.macro  RESTORE_GENERAL_REGS_EXCEPT_A0_T0
    //  a0直接通过函数返回回来的值, t0作为临时用一下的寄存器办一些其他事
    //  risc-v中没有汇编层面定死的栈指针, 所以不用担心sp的问题
    REG_L    x1, 0*REGBYTES(a0)
    REG_L    x2, 1*REGBYTES(a0)
    REG_L    x3, 2*REGBYTES(a0)
    //REG_L    x4, 3*REGBYTES(a0)   //  tp不保存
    //REG_L    x5, 4*REGBYTES(a0)   //  t0
    REG_L    x6, 5*REGBYTES(a0)
    REG_L    x7, 6*REGBYTES(a0)
    REG_L    x8, 7*REGBYTES(a0)
    REG_L    x9, 8*REGBYTES(a0)
    //REG_L    x10, 9*REGBYTES(a0)  //  a0本身
    REG_L    x11, 10*REGBYTES(a0)
    REG_L    x12, 11*REGBYTES(a0)
    REG_L    x13, 12*REGBYTES(a0)
    REG_L    x14, 13*REGBYTES(a0)
    REG_L    x15, 14*REGBYTES(a0)
    REG_L    x16, 15*REGBYTES(a0)
    REG_L    x17, 16*REGBYTES(a0)
    REG_L    x18, 17*REGBYTES(a0)
    REG_L    x19, 18*REGBYTES(a0)
    REG_L    x20, 19*REGBYTES(a0)
    REG_L    x21, 20*REGBYTES(a0)
    REG_L    x22, 21*REGBYTES(a0)
    REG_L    x23, 22*REGBYTES(a0)
    REG_L    x24, 23*REGBYTES(a0)
    REG_L    x25, 24*REGBYTES(a0)
    REG_L    x26, 25*REGBYTES(a0)
    REG_L    x27, 26*REGBYTES(a0)
    REG_L    x28, 27*REGBYTES(a0)
    REG_L    x29, 28*REGBYTES(a0)
    REG_L    x30, 29*REGBYTES(a0)
    REG_L    x31, 30*REGBYTES(a0)
.endm

.macro  RESTORE_SEPC_SSTATUS
    REG_L   t0, 31*REGBYTES(a0)
    csrw    sepc, t0
    REG_L   t0, 32*REGBYTES(a0)
    csrw    sstatus, t0
.endm

.macro TRAP_RESTORE_A0_T0
    REG_L    t0, 4*REGBYTES(a0)
    REG_L    a0, 9*REGBYTES(a0)
.endm

//  由于这两个部分没有参考openSBI, 有一些担心, 单独拿出来
.macro  SAVE_SCAUSE
    csrr    t0, scause
    REG_S   t0, 33*REGBYTES(sp)
.endm

.macro  RESTORE_SCAUSE
    REG_L   t0, 33*REGBYTES(a0)
    csrw    scause, t0
.endm

.text
.global trap_vector
.align 2    //  4 bytes 对齐
trap_vector:
    SETUP_SP_AND_SAVE_ORIGINAL_SP

    SAVE_GENERAL_REGS_EXCEPT_SP

    SAVE_SEPC_SSTATUS

    SAVE_SCAUSE

    mv      a0, sp
    call    trap_handler

.global post_trap_handler   //  用于给进程提供函数
post_trap_handler:

trap_exit:
    RESTORE_GENERAL_REGS_EXCEPT_A0_T0

    RESTORE_SEPC_SSTATUS

    RESTORE_SCAUSE

    TRAP_RESTORE_A0_T0

    sret
