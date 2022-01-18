//
//  defs.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/18.
//

#ifndef defs_h
#define defs_h

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

#endif /* defs_h */
