//
//  bcache.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/5/26.
//

#include "bcache.h"
#include "spinlock.h"

#include "virtio.h"

//  读具体哪一个块应该直接用函数传递给设备
//  而不是从buf结构体中获取信息
//  但时间不够了

extern void panic(char * s);

#define NBUF 30

struct {
    struct spinlock lock;
    struct buf buf[NBUF];
    struct buf head;
} bcache;

void binit(void) {
    struct buf *b;
    
    initlock(&bcache.lock, "bcache");
    
    bcache.head.prev = &bcache.head;
    bcache.head.next = &bcache.head;
    for(b = bcache.buf; b < bcache.buf+NBUF; b++){
        b->next = bcache.head.next;
        b->prev = &bcache.head;
        initsleeplock(&b->lock, "buffer");
        bcache.head.next->prev = b;
        bcache.head.next = b;
    }
}


struct buf* bget(unsigned int dev, unsigned int blockno) {
    struct buf *b;
    
    acquire(&bcache.lock);
    
    // Is the block already cached?
    for(b = bcache.head.next; b != &bcache.head; b = b->next){
        if(b->dev == dev && b->blockno == blockno){
            b->refcnt++;
            release(&bcache.lock);
            acquiresleep(&b->lock);
            return b;
        }
    }
    
    // Not cached.
    // Recycle the least recently used (LRU) unused buffer.
    for(b = bcache.head.prev; b != &bcache.head; b = b->prev){
        if(b->refcnt == 0) {
            b->dev = dev;
            b->blockno = blockno;
            b->valid = 0;
            b->refcnt = 1;
            release(&bcache.lock);
            acquiresleep(&b->lock);
            return b;
        }
    }
    panic("bget: no buffers");
    return 0;
}

struct buf * bread(unsigned int dev, unsigned int blockno) {
    struct buf *b;
    
    b = bget(dev, blockno);
    if(!b->valid) {
        virtio_disk_rw(b, 0);
        b->valid = 1;
    }
    return b;
}

//  这个buf必须上锁
void bwrite(struct buf *b) {
    virtio_disk_rw(b, 1);
}

void brelse(struct buf *b) {    
    releasesleep(&b->lock);
    
    acquire(&bcache.lock);
    b->refcnt--;
    if (b->refcnt == 0) {
        // no one is waiting for it.
        b->next->prev = b->prev;
        b->prev->next = b->next;
        b->next = bcache.head.next;
        b->prev = &bcache.head;
        bcache.head.next->prev = b;
        bcache.head.next = b;
    }
    
    release(&bcache.lock);
}
