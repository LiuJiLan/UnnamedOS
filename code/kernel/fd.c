//
//  fd.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/5/23.
//

#include "fd.h"
#include "param.h"
#include "spinlock.h"
#include "file.h"

extern void panic(char * s);

void fd_init(struct fde * fdtbl) {
    //  其实不需要, 因为proc初始化的时候已经清空了
}

void fd_fork(struct fde * pfdtbl, struct fde * cfdtbl) {
    struct file * fl = NULL;
    for (int i = 0; i < NFD; i++) {
        if (pfdtbl[i].isOpened == FDE_OPEN) {
            fl = pfdtbl[i].file;
            file_increase(fl);
            cfdtbl[i] = pfdtbl[i];
        }
    }
}

void fd_clear(struct fde * fdtbl) {
    struct file * fl = NULL;
    for (int i = 0; i < NFD; i++) {
        if (fdtbl[i].isOpened == FDE_OPEN) {
            fl = fdtbl[i].file;
            file_decrease(fl);
        }
    }
}

void sys_read(struct proc * proc) {
    int fd = (int)proc->context.a0;
    regs_t ret = 0;
    if (fd < 0 || fd >= NFD) {
        ret = -1;
    } else if (proc->fdtbl[fd].isOpened == FDE_CLOSE) {
        ret = -1;
    } else if (proc->fdtbl[fd].flags & O_WRONLY) {
        ret = -1;   //  读一个只写文件
    } else {
        ret = file_read(proc->fdtbl[fd].file, proc);
    }
    
    proc->context.a0 = ret;
    proc->context.sepc += 4;
    return;
}

void sys_write(struct proc * proc) {
    int fd = (int)proc->context.a0;
    regs_t ret = 0;
    if (fd < 0 || fd >= NFD) {
        ret = -1;
    } else if (proc->fdtbl[fd].isOpened == FDE_CLOSE) {
        ret = -1;
    } else if (proc->fdtbl[fd].flags == O_RDONLY) {
        ret = -1;   //  读一个只读文件
    } else {
        ret = file_write(proc->fdtbl[fd].file, proc);
    }
    
    proc->context.a0 = ret;
    proc->context.sepc += 4;
    return;
}
