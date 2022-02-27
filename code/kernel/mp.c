//
//  mp.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/2/22.
//

#include "type.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
//#include "mp.h"
//#include "x86.h"
#include "mmu.h"
#include "proc.h"

struct cpu cpus[NCPU];
int ncpu;
int ismp;

void mpinit(void) {
    //int ismp;
    //  我觉得这个值应该放在外面
    ismp = 1;
    
    //  xv6用的是floating pointer的方法
    //  我们应该采用的是读dtb的方法
    //  我们暂时没有做读取
    ncpu = 0;
    //我们在此初始化只是为了提醒自己
    for (int i = 0; i < NCPU; i++) {
        cpus[i].hart_id = i;
        ncpu++;
    }
}
