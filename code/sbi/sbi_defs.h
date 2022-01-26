//
//  sbi_defs.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/21.
//

#ifndef sbi_defs_h
#define sbi_defs_h

#include "sbi_type.h"

//  temp
void panic(char * s);

//  loader.c
uptr_t loader_kernel(void);

#endif /* sbi_defs_h */
