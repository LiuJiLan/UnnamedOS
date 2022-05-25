//
//  ccache.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/5/22.
//

//  char device cache
//  注意, 本质上跟uart是一个整体

#ifndef ccache_h
#define ccache_h

#include "types.h"
#include "vm.h"

void cdev_init(void);
ssize_t cdev_write(pgtbl_t upgtbl, uptr_t uva, size_t count);
ssize_t cdev_read(pgtbl_t upgtbl, uptr_t uva, size_t count);
void cdev_read_handle_intr(char ch);

#endif /* ccache_h */
