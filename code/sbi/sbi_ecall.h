//
//  sbi_ecall.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/31.
//

#ifndef sbi_ecall_h
#define sbi_ecall_h

#include "sbi_trap.h"
#include "sbi_type.h"

int sbi_ecall_handler(struct sbi_trap_regs * regs);
//  返回值非0代表有问题

//  EID部分
#define Base_Extension                          0x10
#define Set_Timer                               0x00
#define Console_Putchar                         0x01
#define Console_Getchar                         0x02
#define Clear_IPI                               0x03
#define Send_IPI                                0x04
#define Remote_FENCE_I                          0x05
#define Remote_SFENCE_VMA                       0x06
#define Remote_SFENCE_VMA_with_ASID             0x07
#define System_Shutdown                         0x08
#define Timer_Extension                         0x54494D45
#define IPI_Extension                           0x735049
#define RFENCE_Extension                        0x52464E43
#define Hart_State_Management_Extension         0x48534D
#define System_Reset_Extension                  0x53525354
#define Performance_Monitoring_Unit_Extension   0x504D55


//  Standard SBI Errors
#define SBI_SUCCESS                 0
#define SBI_ERR_FAILED              -1
#define SBI_ERR_NOT_SUPPORTED       -2
#define SBI_ERR_INVALID_PARAM       -3
#define SBI_ERR_DENIED              -4
#define SBI_ERR_INVALID_ADDRESS     -5
#define SBI_ERR_ALREADY_AVAILABLE   -6
#define SBI_ERR_ALREADY_STARTED     -7
#define SBI_ERR_ALREADY_STOPPED     -8

#endif /* sbi_ecall_h */
