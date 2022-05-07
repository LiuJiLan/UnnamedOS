//
//  kalloc.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/4/24.
//

#include "kalloc.h"
#include "spinlock.h"
#include "types.h"
#include "string.h"

struct run {    //  链表
    struct run * next;
};

struct {
    struct spinlock lock;
    struct run * freelist;
} kmem; //  以页为单位的内存池

void kalloc_init(void) {
    initlock(&kmem.lock, "kmem");
    kmem.freelist = NULL;
}

char * kalloc(void) {
    struct run *r;
    
    acquire(&kmem.lock);
    
    r = kmem.freelist;
    if (r) {    //  如果不是NULL就分出一个node出来
        kmem.freelist = r->next;
    }
    
    release(&kmem.lock);
    
    return (char *)r;
}

void kfree(char * v) {
    struct run *r;
    
    memset(v, 0x66, PGSIZE);
    
    /*
     int i = 0;
     unsigned char * p = v;
     int xian_bei[4] = {0x14, 0x45, 0x11, 0x00};
     //  小端序0x114514
     
     while (i < PGSIZE) {
     *p = xian_bei[i % 4];
     p++;
     i++;
     }
     */
    
    acquire(&kmem.lock);
    
    r = (struct run*)v;
    r->next = kmem.freelist;
    kmem.freelist = r;
    
    release(&kmem.lock);
}
