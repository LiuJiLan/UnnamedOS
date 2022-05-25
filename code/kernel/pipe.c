//
//  pipe.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/5/25.
//

#include "pipe.h"

void pipe_init(struct pipe * pi) {
    pi->head = 0;
    pi->tail = -1;
    initlock(&pi->lock, "pipe");
}

int pipe_isEmpty(struct pipe * pi) {
    return pi->tail == -1;
}

int pipe_isFull(struct pipe * pi) {
    //    return !isEmpty() && (tail + 1) % maxSize == head;
    return pi->tail != -1 && (pi->tail + 1) % PIPE_SZ == pi->head;
}

int pipe_push(struct pipe * pi, char val) {    //    push的同时返回是否成功
    if (pipe_isEmpty(pi)) {
        return 0;
    }
    pi->tail = (pi->tail + 1) % PIPE_SZ;
    pi->data[pi->tail] = val;
    return 1;
}

int pipe_pop(struct pipe * pi) {    //    只pop, 不会返回内容
    if (pipe_isEmpty(pi)) {
        return 0;
    }
    if (pi->head == pi->tail) {
        pi->head = 0;
        pi->tail = -1;
    } else {
        pi->head = (pi->head + 1) % PIPE_SZ;
    }
    return 1;
}

char pipe_get(struct pipe * pi) {    //    获取头部元素, 不会同时pop
    if (pipe_isEmpty(pi)) {
        return -1;
    } else {
        return pi->data[pi->head];
    }
}

ssize_t pipe_write(struct pipe * pi, pgtbl_t upgtbl, uptr_t uva, size_t count) {
    ssize_t cnt = 0;
    
    acquire(&pi->lock);
    
    char ch = 0;
    int ret = 0;    //  接受传输是否正确的返回值
    while (count--) {
        ret = vm_memmove(upgtbl, (uptr_t)&ch, uva, 1, 1);
        if (ret == -1) {
            return -1;
        }
        
        if (pipe_isFull(pi)) {
            return cnt;
        }
        pipe_push(pi, ch);
        cnt++;
        uva++;   //  sizeof(char)
    }
    
    release(&pi->lock);
    
    return cnt;
}

ssize_t pipe_read(struct pipe * pi, pgtbl_t upgtbl, uptr_t uva, size_t count) {
    ssize_t cnt = 0;
    
    acquire(&pi->lock);
    
    char ch = 0;
    int ret = 0;    //  接受传输是否正确的返回值
    while (count--) {
        if (pipe_isEmpty(pi)) {
            return cnt;
        }
        ch = pipe_get(pi);
        ret = vm_memmove(upgtbl, (uptr_t)&ch, uva, 1, 0);
        if (ret == -1) {
            return -1;
        }
        cnt++;
        uva++;   //  sizeof(char)
        pipe_pop(pi);
    }
    
    release(&pi->lock);
    
    return cnt;
}
