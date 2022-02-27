//
//  cpu.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/2/26.
//

#include "cpu.h"
#include "sbi_defs.h"

struct cpu cpus[NCPU];
int ncpu;

struct cpu* mycpu(void) {
    //其实不太需要,
    //因为对于risc-v而言有hart_id来识别
    int i = get_mhartid();
    return &cpus[i];
}

//void cpu_init(void) {
//
//}
