//
//  kalloc.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/17.
//

//  暂时没有写lock相关的, 暂时有关的部分都给注释掉

#include "defs.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"

void freerange(void * vstart, void * vend);

extern char end[];  //  在ld中定义, 更多详情见README/main.c/引入外部symbol

struct run {
    struct run *next;
};

struct {
    struct spinlock lock;
    int use_lock;
    struct run *freelist;
} kmem;

void kinit1(void * vstart, void * vend) {
    initlock(&kmem.lock, "kmem");
    kmem.use_lock = 0;
    freerange(vstart, vend);
}

void kinit2(void * vstart, void * vend) {
    freerange(vstart, vend);
    kmem.use_lock = 1;
}

void freerange(void *vstart, void *vend) {
    char * p;
    //  char * 和unsigned char * 都是8位指针, 由于memset的指针用的是char * , 所以这里我们也用char *
    p = (char *)PGROUNDUP((unsigned long)vstart);
    for(; p + PGSIZE <= (char *)vend; p += PGSIZE) {
        kfree(p);
    }
}

void kfree(char *v) {
    struct run *r;
    
    if((unsigned long)v % PGSIZE || (void *)v < (void *)end || V2P(v) >= PHYSTOP) {
        panic("kfree");
    }
    
    //  填入一些垃圾数据
    //  想整活但是其实int还是当char使的
    //  希望以后能初始化0x114514(恼)
    memset(v, 0x66, PGSIZE);
    
    // 解开下面的注释以获得赏心悦目的初始化内存(乐)
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
    
    
    if (kmem.use_lock) {
        acquire(&kmem.lock);
    }
    
    r = (struct run*)v;
    r->next = kmem.freelist;
    kmem.freelist = r;
    
    if (kmem.use_lock) {
        release(&kmem.lock);
    }
    
    //  注意此处的处理, 我们认为kmem.freelist的初值是NULL(0)
    //  它之后都指向链表的第一个node
    //  简而言之就是不停向dummy head和和第一个node直接插入新的node
}

char* kalloc(void) {
    struct run *r;
    
    if (kmem.use_lock) {
        acquire(&kmem.lock);
    }
    
    r = kmem.freelist;
    if (r) {    //  如果不是NULL就分出一个node出来
        kmem.freelist = r->next;
    }
    
    if (kmem.use_lock) {
        release(&kmem.lock);
    }
    
    return (char*)r;
}
