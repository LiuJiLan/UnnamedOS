//
//  proc.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/2/22.
//

#ifndef proc_h
#define proc_h

#include "param.h"

struct cpu {
    //uchar apicid;                // Local APIC ID
    //struct context *scheduler;   // swtch() here to enter scheduler
    //struct taskstate ts;         // Used by x86 to find stack for interrupt
    //struct segdesc gdt[NSEGS];   // x86 global descriptor table
    volatile unsigned int started;       // Has the CPU started?
    int ncli;                    // Depth of pushcli nesting.
    int intena;                  // Were interrupts enabled before pushcli?
    //struct proc *proc;           // The process running on this cpu or null
    
    int hart_id;    //  for debug
};

/*
 ncli与intena
 由于上锁时要关中断, 而又可能存在多个锁
 只有当所有的锁都释放后才会恢复到初次上锁前的状态
 (有可能一开始就没有开中断, 所以用intena去记录这个状态)
 */

extern struct cpu cpus[NCPU];
extern int ncpu;

#endif /* proc_h */
