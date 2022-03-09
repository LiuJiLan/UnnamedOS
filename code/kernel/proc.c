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

static struct proc *initproc;

int nextpid = 1;
extern void forkret(void);
extern void trapret(void);

static void wakeup1(void *chan);

void pinit(void) {
    initlock(&ptable.lock, "ptable");
}

// Must be called with interrupts disabled
//  注意在xv6中是调用了mycpu()来处理, 之后还是会处理的
int cpuid(void) {
    return (int)get_mhartid();
}

// Must be called with interrupts disabled to avoid the caller being
// rescheduled between reading lapicid and running through the loop.
struct cpu* mycpu(void) {
    //其实不太需要,
    //因为对于risc-v而言有hart_id来识别
    int i = get_mhartid();
    return &cpus[i];
}

// Disable interrupts so that we are not rescheduled
// while reading proc from the cpu structure
struct proc* myproc(void) {
    struct cpu *c;
    struct proc *p;
    pushcli();
    c = mycpu();
    p = c->proc;
    popcli();
    return p;
}


// Look in the process table for an UNUSED proc.
// If found, change state to EMBRYO and initialize
// state required to run in the kernel.
// Otherwise return 0.
static struct proc* allocproc(void) {
    struct proc *p;
    char *sp;
    
    acquire(&ptable.lock);
    
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
        if (p->state == UNUSED) {
            goto found;
        }
    }
    
    release(&ptable.lock);
    return 0;
    
found:
    p->state = EMBRYO;
    p->pid = nextpid++;
    
    //  以下部分根据xv6-riscv更改
    
    //  release(&ptable.lock);
    //  release的位置也需要改动
    
    
//  xv6-riscv版本中的分配有所不同
//  xv6的分配本质:

//  +-------------------+ <- high address = low address + KSTACKSIZE
//  |     trapframe     |
//  | sizeof(trapframe) |
//  +-------------------+ <- p->tf
//  |      trapret      |
//  |  4bytes / 32bits  |
//  +-------------------+
//  |      context      |
//  |  sizeof(context)  |
//  +-------------------+ <- p->context
//  |                   |
//  .       blank       .
//  |                   |
//  +-------------------+ <- p->kstack = low address
//    // Allocate kernel stack.
//    if((p->kstack = kalloc()) == 0){
//        p->state = UNUSED;
//        return 0;
//    }
//    sp = p->kstack + KSTACKSIZE;
//
//    // Leave room for trap frame.
//    sp -= sizeof *p->tf;
//    p->tf = (struct trapframe*)sp;
//
//    // Set up new context to start executing at forkret,
//    // which returns to trapret.
//    sp -= 4;
//    *(uint*)sp = (uint)trapret;
//
//    sp -= sizeof *p->context;
//    p->context = (struct context*)sp;
//    memset(p->context, 0, sizeof *p->context);
//    p->context->eip = (uint)forkret;
    
    return p;
}
