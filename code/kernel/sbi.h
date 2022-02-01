//
//  sbi.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/27.
//

#ifndef sbi_h
#define sbi_h

#include "type.h"

struct sbiret {
    long error;
    long value;
};

// Base Extension
struct sbiret sbi_get_sbi_spec_version(void);
struct sbiret sbi_get_sbi_impl_id(void);
struct sbiret sbi_get_sbi_impl_version(void);
struct sbiret sbi_probe_extension(regs_t EID);
struct sbiret sbi_get_mvendorid(void);
struct sbiret sbi_get_marchid(void);
struct sbiret sbi_get_mimpid(void);

#endif /* sbi_h */
