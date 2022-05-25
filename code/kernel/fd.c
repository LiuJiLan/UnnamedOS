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
#include "vm.h"
#include "types.h"

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
            fdtbl[i].isOpened = FDE_CLOSE;
            fl = fdtbl[i].file;
            file_decrease(fl);
        }
    }
}

//  没找到返回-1
int fd_find_usable_to_use(struct fde * fdtbl) {
    for (int i = 0; i < NFD; i++) {
        if (fdtbl[i].isOpened == FDE_CLOSE) {
            fdtbl[i].isOpened = FDE_OPEN;
            return i;
        }
    }
    return -1;
}

void sys_read(struct proc * proc) {
    int fd = (int)proc->context.a0;
    regs_t ret = 0;
    struct fde * fdtbl = proc->fdtbl;
    if (fd < 0 || fd >= NFD) {
        ret = -1;
    } else if (fdtbl[fd].isOpened == FDE_CLOSE) {
        ret = -1;
    } else if (fdtbl[fd].flags & O_WRONLY) {
        ret = -1;   //  读一个只写文件
    } else {
        ret = file_read(fdtbl[fd].file, proc);
    }
    
    proc->context.a0 = ret;
    proc->context.sepc += 4;
    return;
}

void sys_write(struct proc * proc) {
    int fd = (int)proc->context.a0;
    regs_t ret = 0;
    struct fde * fdtbl = proc->fdtbl;
    if (fd < 0 || fd >= NFD) {
        ret = -1;
    } else if (fdtbl[fd].isOpened == FDE_CLOSE) {
        ret = -1;
    } else if (fdtbl[fd].flags == O_RDONLY) {
        ret = -1;   //  读一个只读文件
    } else {
        ret = file_write(fdtbl[fd].file, proc);
    }
    
    proc->context.a0 = ret;
    proc->context.sepc += 4;
    return;
}

void sys_close(struct proc * proc) {
    int fd = (int)proc->context.a0;
    regs_t ret = 0;
    struct fde * fdtbl = proc->fdtbl;
    if (fd < 0 || fd >= NFD) {
        ret = -1;
    } else if (fdtbl[fd].isOpened == FDE_CLOSE) {
        ret = -1;
    } else {
        file_decrease(fdtbl[fd].file);
        fdtbl[fd].isOpened = FDE_CLOSE;
        ret = 0;
    }
    
    proc->context.a0 = ret;
    proc->context.sepc += 4;
    return;
}

void sys_pipe2(struct proc * proc) {
    //  BUG!!!
    //  临时实现!!!!!
    //  定死返回3和4, 在proc装载init进程的时候完成
    int ret[2] = {3, 4};
    vm_memmove(proc->upgtbl, (uptr_t)ret, proc->context.a0, sizeof(int) * 2, 0);
    
    proc->context.a0 = 0;   //  一定成功
    proc->context.sepc += 4;
    return;
}

void sys_dup(struct proc * proc) {
    int fd = (int)proc->context.a0;
    regs_t ret = 0;
    struct fde * fdtbl = proc->fdtbl;
    
    if (fd < 0 || fd >= NFD) {
        ret = -1;
    } else {
        int new = fd_find_usable_to_use(proc->fdtbl);
        if (new == -1) {
            ret = -1;
        } else {
            struct file * fl = fdtbl[fd].file;
            if (!fl) {
                if (fl->usable == F_UNUSABLE) {
                    file_increase(fl);
                }
            }
            fdtbl[new] = fdtbl[fd];
            ret = new;
        }
    }
    
    proc->context.a0 = ret;
    proc->context.sepc += 4;
    return;
}


void sys_dup3(struct proc * proc) {
    //  BUG!!!
    //  忽略了flags
    int old_fd = (int)proc->context.a0;
    int new_fd = (int)proc->context.a1;
    regs_t ret = 0;
    struct fde * fdtbl = proc->fdtbl;
    
    if (old_fd < 0 || old_fd >= NFD || new_fd < 0 || new_fd >= NFD) {
        ret = -1;
    } else if (new_fd == old_fd) {
        ret = new_fd;
    } else {
        struct file * fl = fdtbl[new_fd].file;
        if (!fl) {
            if (fl->usable == F_UNUSABLE) {
                file_decrease(fl);
            }
        }
        fdtbl[new_fd] = fdtbl[old_fd];
        ret = new_fd;
    }
    
    proc->context.a0 = ret;
    proc->context.sepc += 4;
    return;
}
