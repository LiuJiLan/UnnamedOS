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
#include "types.h"
#include "proc.h"

struct file {
    enum {REGULAR, DIRECTORY, SYMLINK, CDEV, BDEV, PIPE, SOCKET} type;
    int ref; //  reference count
    volatile enum {F_USABLE = 0, F_UNUSABLE} usable;    //  和proc结构体里的冲突了
    //  struct spinlock lock;   //  其实不需要每个表项都要一个自旋锁
};

struct {
    struct spinlock lock;
    struct file filetbl[NFILE];
} kftable;

void file_init(void);
void file_increase(struct file * f);
void file_decrease(struct file * f);
ssize_t file_read(struct file * f, struct proc * proc);
ssize_t file_write(struct file * f, struct proc * proc);

#endif /* file_h */
