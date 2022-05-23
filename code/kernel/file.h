//
//  file.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/5/18.
//

#ifndef file_h
#define file_h

#include "spinlock.h"
#include "param.h"

struct file {
    enum {REGULAR, DIRECTORY, SYMLINK, CDEV, BDEV, PIPE, SOCKET} type;
    int ref; //  reference count
    volatile enum {F_UNUSABLE, F_USABLE} usable;    //  和proc结构体里的冲突了
    struct spinlock lock;
};

struct {
    struct spinlock lock;
    struct file filetbl[NFILE];
} file_table;

#endif /* file_h */
