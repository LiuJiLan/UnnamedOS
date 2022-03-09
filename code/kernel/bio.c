//
//  bio.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/3/5.
//

//  与书上代码版本有差别的时间线在在Commits on Sep 12, 2016

//  xv6在此处做了一些说明, 我们直接搬过来:

// Buffer cache.
//
// The buffer cache is a linked list of buf structures holding
// cached copies of disk block contents.  Caching disk blocks
// in memory reduces the number of disk reads and also provides
// a synchronization point for disk blocks used by multiple processes.
//
// Interface:
// * To get a buffer for a particular disk block, call bread.
// * After changing buffer data, call bwrite to write it to disk.
// * When done with the buffer, call brelse.
// * Do not use the buffer after calling brelse.
// * Only one process at a time can use a buffer,
//     so do not keep them longer than necessary.
//
// The implementation uses two state flags internally:
// * B_VALID: the buffer data has been read from the disk.
// * B_DIRTY: the buffer data has been modified
//     and needs to be written to disk.

#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "buf.h"

struct {
    struct spinlock lock;
    struct buf buf[NBUF];
    
    // Linked list of all buffers, through prev/next.
    // head.next is most recently used.
    struct buf head;
} bcache;

void binit(void) {
    struct buf *b;
    
    initlock(&bcache.lock, "bcache");
    
    // Create linked list of buffers
    bcache.head.prev = &bcache.head;
    bcache.head.next = &bcache.head;
    for(b = bcache.buf; b < bcache.buf+NBUF; b++){
        b->next = bcache.head.next;
        b->prev = &bcache.head;
        initsleeplock(&b->lock, "buffer");
        //  书中的版本上一行是:
        //  b->dev = -1;
        //  查看GitHub记录可以看出
        //  截止Commits on Aug 25, 2016, 只有b->dev = -1;一行
        //  在Commits on Sep 12, 2016的两次更新中
        //  第一次加入了initsleeplock(...);
        //  第二次删除了b->dev = -1;变成了现在的状态
        bcache.head.next->prev = b;
        bcache.head.next = b;
    }
    
    //  这个循环的作用就是不断把新的node插入到head与之前与head最近的node之间
    //  所以在循环之前会把head指向自己
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
static struct buf* bget(unsigned int dev, unsigned int blockno) {
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
    
    // Not cached; recycle an unused buffer.
    // Even if refcnt==0, B_DIRTY indicates a buffer is in use
    // because log.c has modified it but not yet committed it.
    for(b = bcache.head.prev; b != &bcache.head; b = b->prev){
        if(b->refcnt == 0 && (b->flags & B_DIRTY) == 0) {
            b->dev = dev;
            b->blockno = blockno;
            b->flags = 0;
            b->refcnt = 1;
            release(&bcache.lock);
            acquiresleep(&b->lock);
            return b;
        }
    }
    panic("bget: no buffers");
}





