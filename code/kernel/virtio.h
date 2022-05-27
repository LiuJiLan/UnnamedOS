//
//  virtio.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/5/26.
//

#ifndef virtio_h
#define virtio_h

#include "bcache.h"

#define VIRTIO0_BASE 0x10001000
#define VIRTIO0_SIZE 0x1000

#define VIRTIO0_IRQ 1

void virtio_disk_rw(struct buf *b, int write);
void virtio_disk_intr(void);

#endif /* virtio_h */
