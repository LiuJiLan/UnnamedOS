//
//  sbi_param.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/26.
//

#ifndef sbi_param_h
#define sbi_param_h

#define MSCRATCH_TEMP_NUM 2

#define SBI_TRAP_REGS_SIZE 33 // 除去zero的31个通用寄存器 + mepc + mstatus

#define SBI_STACK_SIZE 1024


#endif /* sbi_param_h */
