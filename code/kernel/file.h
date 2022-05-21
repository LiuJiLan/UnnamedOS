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
    volatile enum {UNUSABLE, USABLE} usable;
    struct spinlock lock;
};

struct {
    struct spinlock lock;
    struct file filetbl[NFILE];
} file_table;

#endif /* file_h */
