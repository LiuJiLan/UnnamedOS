//
//  sbi.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/27.
//

#ifndef sbi_h
#define sbi_h

struct sbiret {
    long error;
    long value;
};

struct sbiret sbi_ecall(unsigned long EID, unsigned long FID);

#endif /* sbi_h */
