//
//  sbi_console.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/2/7.
//

#include "sbi_console.h"

int dummy_putchar(int ch) {
    //  ignore any input
    return -1;  //  EOF
}

int dummy_getchar(void) {
    return -1;  //  EOF
}

void console_init(void) {
    //  因为不同的hart都可能会调用输出, 所以这是一个临界区
    //  我们理应声明一个lock, 只是我们现在单hart暂时没有写
    //  此处锁的设计之后会参考xv-6的console
    
    //  lock()
    
    console_putchar = &dummy_putchar;
    console_getchar = &dummy_getchar;
}

void set_console_putchar(void * func) {
    if (func == 0) {
        console_putchar = &dummy_putchar;
    } else {
        console_putchar = func;
    }
}

void set_console_getchar(void * func) {
    if (func == 0) {
        console_getchar = &dummy_getchar;
    } else {
        console_getchar = func;
    }
}
