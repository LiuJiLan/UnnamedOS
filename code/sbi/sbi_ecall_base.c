//
//  sbi_ecall_base.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/31.
//

#include "sbi_ecall_base.h"

long sbi_get_sbi_spec_version(void);
long sbi_get_sbi_impl_id(void);
long sbi_get_sbi_impl_version(void);
long sbi_probe_extension(regs_t EID);
long sbi_get_mvendorid(void);
long sbi_get_marchid(void);
long sbi_get_mimpid(void);

int sbi_ecall_base_handler(regs_t FID, long * error, long * value,\
                           struct sbi_trap_regs * regs) {
    *error = SBI_SUCCESS;
    switch (FID) {
        case SBI_GET_SBI_SPEC_VERSION:
            *value = sbi_get_sbi_spec_version();
            break;
            
        case SBI_GET_SBI_IMPL_ID:
            *value = sbi_get_sbi_impl_id();
            break;
            
        case SBI_GET_SBI_IMPL_VERSION:
            *value = sbi_get_sbi_impl_version();
            break;
            
        case SBI_PROBE_EXTENSION:
            *value = sbi_probe_extension(regs->a0);
            break;
            
        case SBI_GET_MVENDORID:
            *value = sbi_get_mvendorid();
            break;
            
        case SBI_GET_MARCHID:
            *value = sbi_get_marchid();
            break;
            
        case SBI_GET_MIMPID:
            *value = sbi_get_mimpid();
            break;
            
        default:
            break;
    }
    return 0;
}

long sbi_get_sbi_spec_version(void) {
    return 0;
}

long sbi_get_sbi_impl_id(void) {
    return -1;
}

long sbi_get_sbi_impl_version(void) {
    return 0;
}

long sbi_probe_extension(regs_t EID) {
    switch (EID) {
        case Set_Timer:
        case Console_Putchar:
        case Console_Getchar:
        case Clear_IPI:
        case Send_IPI:
        case Remote_FENCE_I:
        case Remote_SFENCE_VMA:
        case Remote_SFENCE_VMA_with_ASID:
        case System_Shutdown:
            return 0;
            
        case Base_Extension:
            return EID;
            
        case Timer_Extension:
        case IPI_Extension:
        case RFENCE_Extension:
        case Hart_State_Management_Extension:
        case System_Reset_Extension:
        case Performance_Monitoring_Unit_Extension:
            return 0;
            
        default:
            return 0;
    }
}


long sbi_get_mvendorid(void) {
    long mvendorid;
    asm volatile("csrr %0, mvendorid"
                 :"=r"(mvendorid)
                 ://    无输入
                 ://    无损坏
                 );
    return mvendorid;
}

long sbi_get_marchid(void) {
    long marchid;
    asm volatile("csrr %0, mvendorid"
                 :"=r"(marchid)
                 ://    无输入
                 ://    无损坏
                 );
    return marchid;
}

long sbi_get_mimpid(void) {
    long mimpid;
    asm volatile("csrr %0, mvendorid"
                 :"=r"(mimpid)
                 ://    无输入
                 ://    无损坏
                 );
    return mimpid;
}
