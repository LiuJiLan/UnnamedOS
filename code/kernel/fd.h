//
//  fd.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/5/23.
//

//  只是proc中文件描述表的一些方法

#ifndef fd_h
#define fd_h

#include "proc.h"

#define O_RDONLY 0x000
#define O_WRONLY 0x001
#define O_RDWR 0x002

void fd_init(struct fde * fdtbl);
void fd_fork(struct fde * pfdtbl, struct fde * cfdtbl);
void fd_clear(struct fde * fdtbl);

#endif /* fd_h */
