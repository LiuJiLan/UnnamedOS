//
//  sbi_param.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/26.
//

#ifndef sbi_param_h
#define sbi_param_h

#define MSCRATCH_TEMP_NUM 2

#define SBI_TRAP_REGS_SIZE 34
//  除去zero的31个通用寄存器 + mepc + mstatus + mcause
//  不知为何openSBI将32个通用寄存器全部给存了, 但没有存mcause
//  为了我自己方便我选择还是存mcause

#define NCPU        2

#define SBI_STACK_SIZE 1024

#define DEFAULT_TIMER_INTERVAL -1


#endif /* sbi_param_h */
