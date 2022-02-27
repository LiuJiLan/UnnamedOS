//
//  sbi_defs.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/21.
//

#ifndef sbi_defs_h
#define sbi_defs_h

#include "sbi_type.h"
#include "sbi_param.h"
#include "cpu.h"

struct spinlock;
struct cpu;

//  cpu.c
struct cpu* mycpu(void);

//  sbi_main.c
regs_t get_mhartid(void);

//  sbi_spinlock.c
void            acquire(struct spinlock*);
int             holding(struct spinlock*);
void            initlock(struct spinlock*, char*);
void            release(struct spinlock*);
void            pushcli(void);
void            popcli(void);

//  temp
//  void panic(char * s);

//  loader.c
//  uptr_t loader_kernel(void);

#endif /* sbi_defs_h */
