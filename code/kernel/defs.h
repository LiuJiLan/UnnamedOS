//
//  defs.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/18.
//

#ifndef defs_h
#define defs_h

#include "type.h"

// console.c
//void            consoleinit(void);
//void            cprintf(char*, ...);
//void            consoleintr(int(*)(void));
//void            panic(char*) __attribute__((noreturn));
void            panic(char*);   //  暂时用来debug, 未实现
                                //  有些时候当断点用, 暂时还需要返回

//  string.c
int             memcmp(const void*, const void*, unsigned long);
void*           memmove(void*, const void*, unsigned long);
void*           memset(void*, int, unsigned long);
char*           safestrcpy(char*, const char*, int);
int             strlen(const char*);
int             strncmp(const char*, const char*, unsigned long);
char*           strncpy(char*, const char*, int);

// kalloc.c
char*           kalloc(void);
void            kfree(char*);
void            kinit1(void*, void*);
void            kinit2(void*, void*);

// vm.c
//void            seginit(void);
void            kvmalloc(void);
pte_t*          setupkvm(void);
//char*           uva2ka(pde_t*, char*);
//int             allocuvm(pde_t*, uint, uint);
//int             deallocuvm(pde_t*, uint, uint);
//void            freevm(pde_t*);
//void            inituvm(pde_t*, char*, uint);
//int             loaduvm(pde_t*, char*, struct inode*, uint, uint);
//pde_t*          copyuvm(pde_t*, uint);
//void            switchuvm(struct proc*);
void            switchkvm(void);
//int             copyout(pde_t*, uint, void*, uint);
//void            clearpteu(pde_t *pgdir, char *uva);

//  获取定长数组的元素个数
#define NELEM(x) (sizeof(x)/sizeof((x)[0]))

#endif /* defs_h */