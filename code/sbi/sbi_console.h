//
//  sbi_console.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/2/7.
//

#ifndef sbi_console_h
#define sbi_console_h

extern int (* console_putchar)(int);
extern int (* console_getchar)(void);
//  暂时没有取putc和getc的原因是, 在标准库中这两个函数需要指明输出的文件目标

void console_init(void);
void set_console_putchar(void * func);
void set_console_getchar(void * func);

static void printint(int xx, int base, int sign);
void cprintf(char *fmt, ...);
void panic(char *s);


#endif /* sbi_console_h */

/*
 回顾一下作用域的情况
 我们用自己的编译器来处理这个问题时,
 如果单在sbi_console.h中声明两个函数指针, 会引发Mac编译器的重复定义
 会认为在main.o中和sbi_console.o中都有这个函数
 解决方法是, 让那两个函数指针的本体放在sbi_console.c中
 然后在.h文件中声明一份引用(用extern声明去.c文件中找)
 */
