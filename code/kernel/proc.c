//
//  proc.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/2/22.
//

#include "proc.h"
#include "defs.h"
#include "spinlock.h"

struct {
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;

void pinit(void) {
  initlock(&ptable.lock, "ptable");
}

struct cpu* mycpu(void) {
    //其实不太需要,
    //因为对于risc-v而言有hart_id来识别
    int i = get_mhartid();
    return &cpus[i];
}
