//
//  cpu.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/2/26.
//

#ifndef cpu_h
#define cpu_h

#include "sbi_defs.h"
#include "sbi_param.h"

//  这块的结构与kernel部分的proc相似

struct cpu {
    int ncli;
    int intena;
};

extern struct cpu cpus[NCPU];
extern int ncpu;

#endif /* cpu_h */
