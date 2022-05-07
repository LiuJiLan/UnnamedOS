//
//  sbi_trap.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/4/19.
//

#ifndef sbi_trap_h
#define sbi_trap_h

struct sbi_trap_regs {
    unsigned long ra;
    unsigned long sp;
    unsigned long gp;
    unsigned long tp;
    unsigned long t0;
    unsigned long t1;
    unsigned long t2;
    unsigned long s0;
    unsigned long s1;
    unsigned long a0;
    unsigned long a1;
    unsigned long a2;
    unsigned long a3;
    unsigned long a4;
    unsigned long a5;
    unsigned long a6;
    unsigned long a7;
    unsigned long s2;
    unsigned long s3;
    unsigned long s4;
    unsigned long s5;
    unsigned long s6;
    unsigned long s7;
    unsigned long s8;
    unsigned long s9;
    unsigned long s10;
    unsigned long s11;
    unsigned long t3;
    unsigned long t4;
    unsigned long t5;
    unsigned long t6;
    unsigned long mepc;
    unsigned long mstatus;
    unsigned long mcause;
}__attribute__((packed));

struct sbi_trap_info {
    unsigned long epc;
    unsigned long cause;
    unsigned long tval;
    unsigned long tval2;
    unsigned long tinst;
}__attribute__((packed));

struct sbi_trap_regs * sbi_trap_handler(struct sbi_trap_regs * regs);

#define Supervisor_Software_Interrupt       1
#define Machine_Software_Interrupt          3
#define Supervisor_Timer_Interrupt          5
#define Machine_Timer_Interrupt             7
#define Supervisor_External_Interrupt       9
#define Machine_External_Interrupt          11

#define Instruction_Address_Misaligned      0
#define Instruction_Access_Fault            1
#define Illegal_Instruction                 2
#define Breakpoint                          3
#define Load_Address_Aisaligned             4
#define Load_Access_Fault                   5
#define Store_AMO_Address_Misaligned        6
#define Store_AMO_Access_Fault              7
#define Environment_Call_From_U_mode        8
#define Environment_Call_From_S_mode        9
#define Environment_Call_From_M_mode        11
#define Instruction_Page_Fault              12
#define Load_Page_Fault                     13
#define Store_AMO_Page_Fault                15

#endif /* sbi_trap_h */
