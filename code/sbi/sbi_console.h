//
//  sbi_console.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/2/7.
//

#ifndef sbi_console_h
#define sbi_console_h

int (* console_putchar)(int);
int (* console_getchar)(void);
//  暂时没有取putc和getc的原因是, 在标准库中这两个函数需要指明输出的文件目标

void console_init(void);
void set_console_putchar(void * func);
void set_console_getchar(void * func);

#endif /* sbi_console_h */
