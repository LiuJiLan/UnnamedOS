//
//  sbi_main.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/4/18.
//

#include "sbi_types.h"
#include "sbi_riscv.h"
#include "clint.h"

//  来自sbi_trap_handler的符号
extern void trap_vector(void);

void panic(char * s);

void sbimain(void) {
    //  设置mstatus
    //  设置权级转换到S, 并开全局中断
    //  (不管之前如何, 只是重新设置)
    regs_t mstatus = 0;
    mstatus = r_mstatus();
    mstatus &= ~(0x3 << 11);    //  清空MPP
    mstatus |= 0x1 << 11;       //  设置MPP为S-mode
    mstatus |= 0x1 << 7;        //  设置MPIE
    w_mstatus(mstatus);
    
    //  设置mtvec
    //  设置trap转跳位置
    w_mtvec((uptr_t)trap_vector);
    
    //  设置medeleg & mideleg
    //  这两个寄存器都是WARL, 我们设置时就全写1就好了
    //  因为本质上我并不希望M处理太多
    //  但又不能像xv6-riscv那样设置,
    //  因为ecall from S/M mode 还需用来调用SBI
    regs_t medeleg = ~(1 << 9 | 1 << 11);
    w_medeleg(medeleg);
    w_mideleg(~0x0);
    
    
    //  设置mie
    //  防止中断打断, 最后再处理
    
    //  设置mscratch
    //  之前汇编部分设置过了
    
    //  设置mepc
    //  正如汇编中注释的那样, 为了比赛所以也固定了S态的转跳值
    w_mepc(0x80200000UL);
    
    //  设置mcause
    //  不设置, 只是按着草稿走
    
    //  设置pmpaddr0 & pmpcfg0
    //  所有PMP CSR都是WARL
    //  给pmpaddr0为最大值, 54位之上0为WARL, 所以干脆全写1
    w_pmpaddr0(~0x0);
    w_pmpcfg0(0xFUL);   //  A = TOR, X = W = R = 1
    
    //  CLINT是当下设计中一定需要移植的部分
    
    //  设置mtimecmp
    //  注意这个寄存器是内存映射式的
    //  注意:设置最大值不会完全关闭时钟中断
    //  注意与SBI手册时钟相关的部分的区别
    set_CLINT_mtimecmp_infinitely();
    //  沿用了之前的程序, 将mtimecmp设置到最大值
    
    //  设置mie
    regs_t mie = 0;
    mie |= 0x1 << 11 | 0x1 << 7 | 0x1 << 3;
    w_mie(mie);
    
    asm volatile("mret");
}
