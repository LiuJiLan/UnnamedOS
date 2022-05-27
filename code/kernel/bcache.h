//
//  bcache.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/5/26.
//

//  block device cache

#ifndef bcache_h
#define bcache_h

#include "sleeplock.h"
#include "param.h"

struct buf {
    int valid;   //   数据是否已经从硬盘读出
    int disk;    //   块设备是否正在用这块缓存
    unsigned int dev;   //  我们暂时只有一个设备
    unsigned int blockno;
    struct sleeplock lock;
    unsigned int refcnt;
    struct buf *prev; // LRU cache list
    struct buf *next;
    unsigned char data[BSIZE];
};

typedef struct buf bcache_t;    //  为了临时兼容

#endif /* bcache_h */
