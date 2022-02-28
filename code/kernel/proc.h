//
//  proc.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/2/22.
//

#ifndef proc_h
#define proc_h

#include "param.h"
#include "type.h"

struct cpu {
    //uchar apicid;                // Local APIC ID
    //struct context *scheduler;   // swtch() here to enter scheduler
    //struct taskstate ts;         // Used by x86 to find stack for interrupt
    //struct segdesc gdt[NSEGS];   // x86 global descriptor table
    volatile unsigned int started;       // Has the CPU started?
    int ncli;                    // Depth of pushcli nesting.
    int intena;                  // Were interrupts enabled before pushcli?
    //struct proc *proc;           // The process running on this cpu or null
    
    int hart_id;    //  add for debug
};

/*
 ncli与intena
 由于上锁时要关中断, 而又可能存在多个锁
 只有当所有的锁都释放后才会恢复到初次上锁前的状态
 (有可能一开始就没有开中断, 所以用intena去记录这个状态)
 */

extern struct cpu cpus[NCPU];
extern int ncpu;

struct context {
    //
};

enum procstate {UNUSED, EMBRYO, SLEEPING, RUNNABLE, RUNNING, ZOMBIE};


// Per-process state
struct proc {
    uptr_t sz;                     // Size of process memory (bytes)
    pde_t* pgdir;                // Page table
    char *kstack;                // Bottom of kernel stack for this process
    enum procstate state;        // Process state
    int pid;                     // Process ID
    struct proc *parent;         // Parent process
    struct trapframe *tf;        // Trap frame for current syscall
    struct context *context;     // swtch() here to run process
    void *chan;                  // If non-zero, sleeping on chan
    int killed;                  // If non-zero, have been killed
    //struct file *ofile[NOFILE];  // Open files
    //struct inode *cwd;           // Current directory
    char name[16];               // Process name (debugging)
};

#endif /* proc_h */
