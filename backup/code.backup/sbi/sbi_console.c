//
//  sbi_console.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/2/7.
//

#include "sbi_console.h"
#include "sbi_spinlock.h"
#include "sbi_defs.h"
#include "sbi_riscv.h"

#include <stdarg.h>

int (* console_putchar)(int);
int (* console_getchar)(void);

static int panicked = 0;

static struct {
    struct spinlock lock;
    int locking;
} cons;


int dummy_putchar(int ch) {
    //  ignore any input
    return -1;  //  EOF
}

int dummy_getchar(void) {
    return -1;  //  EOF
}

void console_init(void) {
    //这里原来我们打算设置一个lock来避免不同核的调用
    //但是这是一个误区, 对于console_putchar/getchar()
    //我们并不用去担心锁的问题
    //如果每输入输出一个字符就要锁一次和解锁一次
    //这个效率就太低了, 一般都是在上层调用的时候去上锁
    
    //就算在M态中
    //我们也暂时只在大量输出的时候考虑上锁
    
    initlock(&cons.lock, "console");
    
    console_putchar = &dummy_putchar;
    console_getchar = &dummy_getchar;
    
    cons.locking = 1;
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

//  直接抄袭xv6

static void printint(int xx, int base, int sign) {
    static char digits[] = "0123456789abcdef";
    char buf[16];
    int i;
    unsigned int x;
    
    if(sign && (sign = xx < 0))
        x = -xx;
    else
        x = xx;
    
    i = 0;
    do{
        buf[i++] = digits[x % base];
    }while((x /= base) != 0);
    
    if(sign)
        buf[i++] = '-';
    
    while(--i >= 0)
        console_putchar(buf[i]);
}
//PAGEBREAK: 50

// Print to the console. only understands %d, %x, %p, %s.
void cprintf(char *fmt, ...) {
    int i, c, locking;
    //unsigned int *argp;
    char *s;
    
    va_list argp;
    va_start(argp, fmt);
    
    locking = cons.locking;
    if(locking)
        acquire(&cons.lock);
    
    if (fmt == 0)
        panic("null fmt");
    
    //argp = (unsigned int*)(void*)(&fmt + 1);
    for(i = 0; (c = fmt[i] & 0xff) != 0; i++){
        if(c != '%'){
            console_putchar(c);
            continue;
        }
        c = fmt[++i] & 0xff;
        if(c == 0)
            break;
        switch(c){
            case 'd':
                printint(va_arg(argp, int), 10, 1);
                break;
            case 'x':
            case 'p':
                printint(va_arg(argp, int), 16, 0);
                break;
            case 's':
                if((s = (char*)va_arg(argp, char*)) == 0)
                    s = "(null)";
                for(; *s; s++)
                    console_putchar(*s);
                break;
            case '%':
                console_putchar('%');
                break;
            default:
                // Print unknown % sequence to draw attention.
                console_putchar('%');
                console_putchar(c);
                break;
        }
    }
    
    if(locking)
        release(&cons.lock);
}

void panic(char *s) {
    int i;
    //unsigned int pcs[10];
    
    clear_mstatus_MIE();
    cons.locking = 0;
    // use lapiccpunum so that we can call panic from mycpu()
    cprintf("hartid %d: panic: ", get_mhartid());
    cprintf(s);
    cprintf("\n");
    //getcallerpcs(&s, pcs);
    //  for(i=0; i<10; i++)
    //    cprintf(" %p", pcs[i]);
    panicked = 1; // freeze other CPU
    for(;;)
        ;
}
