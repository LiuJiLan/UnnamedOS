//
//  ccache.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/5/22.
//

#include "ccache.h"
#include "sleeplock.h"
#include "uart.h"
#include "proc.h"

extern void panic(char * s);

#define N_CDEV_W_BUF    8
#define N_CDEV_R_BUF    64

struct sleeplock cdev_w_lock;
struct spinlock cdev_r_lock;

//  cdev_w_buf设立的原因不是为了减少对外设的访问
//  而是为了防止每次都只从vm_memmove读取一个char
//  现在先偷一下懒
//  char cdev_w_buf[N_CDEV_W_BUF] = {0};

char cdev_r_buf[N_CDEV_R_BUF];
int cdev_r_buf_head;
int cdev_r_buf_tail;

void cdev_r_buf_init(void) {
    cdev_r_buf_head = 0;
    cdev_r_buf_tail = -1;
}

int cdev_r_buf_is_empty(void) {
    return cdev_r_buf_tail == -1;
}

int cdev_r_buf_is_full(void) {
    //  return !isEmpty() && (tail + 1) % maxSize == head;
    return cdev_r_buf_tail != -1 && (cdev_r_buf_tail + 1) % N_CDEV_R_BUF == cdev_r_buf_head;
}

//  push的同时返回是否成功
int cdev_r_buf_push(char val) {
    if (cdev_r_buf_is_full()) {
        return 0;
    }
    cdev_r_buf_tail = (cdev_r_buf_tail + 1) % N_CDEV_R_BUF;
    cdev_r_buf[cdev_r_buf_tail] = val;
    return 1;
}

//  只pop, 不会返回内容
int cdev_r_buf_pop(void) {
    if (cdev_r_buf_is_empty()) {
        return 0;
    }
    if (cdev_r_buf_head == cdev_r_buf_tail) {
        cdev_r_buf_head = 0;
        cdev_r_buf_tail = -1;
    } else {
        cdev_r_buf_head = (cdev_r_buf_head + 1) % N_CDEV_R_BUF;
    }
    return 1;
}

//  获取头部元素, 不会同时pop
char cdev_r_buf_get(void) {
    if (cdev_r_buf_is_empty()) {
        return -1;
    } else {
        return cdev_r_buf[cdev_r_buf_head];
    }
}



void cdev_init(void) {
    initsleeplock(&cdev_w_lock, "cdev_w_lock");
    initlock(&cdev_r_lock, "cdev_r_lock");
    cdev_r_buf_init();
    
    uart_init();
}

ssize_t cdev_write(pgtbl_t upgtbl, uptr_t uva, size_t count) {
    //  其实我觉得uart写操作不需要上睡眠锁
    //  但是由于putc中有
    //  while ((uart_read_reg(LSR) & LSR_TX_IDLE) == 0);
    //  所以还是上了睡眠锁
    
    //  注意返回的值应该是实际写出去的字节数
    //  我们默认全部字节都写出去了
    //  (但是我们保留了错误返回-1)
    size_t cnt = count;
    
    acquiresleep(&cdev_w_lock);
    
    char ch = 0;
    int ret = 0;    //  接受传输是否正确的返回值
    while (cnt--) {   //  是unsigned, 不用担心传进来的是负数
        ret = vm_memmove(upgtbl, (uptr_t)&ch, uva, 1, 1);
        if (ret == -1) {
            return -1;
        }
        uart_putc(ch);
        uva++;   //  sizeof(char)
    }
    
    releasesleep(&cdev_w_lock);
    
    return count;
}

//  注意, 如果缓存为空则等待中断后重新调度
//  和普通的文件会读到EOF (也就是0)不同
ssize_t cdev_read(pgtbl_t upgtbl, uptr_t uva, size_t count) {
    //  更复杂的情况其实更适合自旋锁
    acquire(&cdev_r_lock);
    if (cdev_r_buf_is_empty()) {
        proc_sleep_for_reason(&cdev_r_lock, cdev_r_buf);
    }
    
    int cnt = 0;
    char ch = 0;
    int ret = 0;    //  接受传输是否正确的返回值
    while (!cdev_r_buf_is_empty()) {
        //  因为之前已经在while中判断了,
        //  所以不用多判断ch是否为-1的情况
        ch = cdev_r_buf_get();
        ret = vm_memmove(upgtbl, (uptr_t)&ch, uva, 1, 0);
        if (ret == -1) {
            return -1;
        }
        cnt++;
        uva++;   //  sizeof(char)
        cdev_r_buf_pop();   //  只有成功的时候才pop
    }
    
    release(&cdev_r_lock);
    
    return cnt;
}

//  注意, 多的输入会被丢弃
void cdev_read_handle_intr(char ch) {
    acquire(&cdev_r_lock);
    
    if (cdev_r_buf_is_full()) {
        cdev_r_buf_pop();
    }
    cdev_r_buf_push(ch);
    
    release(&cdev_r_lock);
}
