//
//  proc.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/2/22.
//

#include "proc.h"
#include "defs.h"

struct cpu* mycpu(void) {
    //其实不太需要,
    //因为对于risc-v而言有hart_id来识别
    int i = get_mhartid();
    return &cpus[i];
}
