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
    struct context *scheduler;   // swtch() here to enter scheduler
    //struct taskstate ts;         // Used by x86 to find stack for interrupt
    //struct segdesc gdt[NSEGS];   // x86 global descriptor table
    volatile unsigned int started;       // Has the CPU started?
    int ncli;                    // Depth of pushcli nesting.
    int intena;                  // Were interrupts enabled before pushcli?
    struct proc *proc;           // The process running on this cpu or null
    
    int hart_id;    //  add for debug
};
//  xv6-riscv中已经弃用了很多部
//  还有一些部分做了更改, 比如pgdir页表的指针被放到了每个proc结构体中

/*
 ncli与intena
 由于上锁时要关中断, 而又可能存在多个锁
 只有当所有的锁都释放后才会恢复到初次上锁前的状态
 (有可能一开始就没有开中断, 所以用intena去记录这个状态)
 */

extern struct cpu cpus[NCPU];
extern int ncpu;

// Saved registers for kernel context switches.
//  参考xv6-riscv(日后称xv6-public为xv6, 以区分)
struct context {
  regs_t ra;
  regs_t sp;

  // callee-saved
  regs_t s0;
  regs_t s1;
  regs_t s2;
  regs_t s3;
  regs_t s4;
  regs_t s5;
  regs_t s6;
  regs_t s7;
  regs_t s8;
  regs_t s9;
  regs_t s10;
  regs_t s11;
};

//  xv6中把这个结构体定义在x86.h中
//  xv6-riscv将其移动到了proc.h中
struct trapframe {
  /*   0 */ regs_t kernel_satp;   // kernel page table
  /*   8 */ regs_t kernel_sp;     // top of process's kernel stack
  /*  16 */ regs_t kernel_trap;   // usertrap()
  /*  24 */ regs_t epc;           // saved user program counter
  /*  32 */ regs_t kernel_hartid; // saved kernel tp
  /*  40 */ regs_t ra;
  /*  48 */ regs_t sp;
  /*  56 */ regs_t gp;
  /*  64 */ regs_t tp;
  /*  72 */ regs_t t0;
  /*  80 */ regs_t t1;
  /*  88 */ regs_t t2;
  /*  96 */ regs_t s0;
  /* 104 */ regs_t s1;
  /* 112 */ regs_t a0;
  /* 120 */ regs_t a1;
  /* 128 */ regs_t a2;
  /* 136 */ regs_t a3;
  /* 144 */ regs_t a4;
  /* 152 */ regs_t a5;
  /* 160 */ regs_t a6;
  /* 168 */ regs_t a7;
  /* 176 */ regs_t s2;
  /* 184 */ regs_t s3;
  /* 192 */ regs_t s4;
  /* 200 */ regs_t s5;
  /* 208 */ regs_t s6;
  /* 216 */ regs_t s7;
  /* 224 */ regs_t s8;
  /* 232 */ regs_t s9;
  /* 240 */ regs_t s10;
  /* 248 */ regs_t s11;
  /* 256 */ regs_t t3;
  /* 264 */ regs_t t4;
  /* 272 */ regs_t t5;
  /* 280 */ regs_t t6;
};

enum procstate {UNUSED, EMBRYO, SLEEPING, RUNNABLE, RUNNING, ZOMBIE};


// Per-process state
struct proc {
    uptr_t sz;                     // Size of process memory (bytes)
    pde_t* pgdir;                // Page table
    //char *kstack;                // Bottom of kernel stack for this process
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
