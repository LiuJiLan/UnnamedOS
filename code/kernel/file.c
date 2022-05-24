//
//  file.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/5/18.
//

#include "file.h"
#include "ccache.h"

extern void panic(char * s);

void file_init(void) {
    initlock(&kftable.lock, "ftable");
    
    //  第一项定死是cdev
    kftable.filetbl[0].type = CDEV;
    kftable.filetbl[0].ref = 1;
    kftable.filetbl[0].usable = F_UNUSABLE;
    
}

void file_increase(struct file * f) {
    acquire(&kftable.lock);
    f->ref++;
    release(&kftable.lock);
}

void file_decrease(struct file * f) {
    acquire(&kftable.lock);
    f->ref--;
    if (--f->ref == 0) {
        f->usable = F_USABLE;
    }
    //  这里要根据类型具体的处理
    switch (f->type) {
        case SOCKET:
            //
            break;
            
        default:
            break;
    }
    release(&kftable.lock);
}

ssize_t file_read(struct file * f, struct proc * proc) {
    switch (f->type) {
        case CDEV:
            //  本来应该是有一个cdev的数组,
            //  我们暂时定死一个
            return cdev_read(proc->upgtbl, proc->context.a1, proc->context.a2);
            break;
            
        default:
            break;
    }
    return -1;
}

ssize_t file_write(struct file * f, struct proc * proc) {
    switch (f->type) {
        case CDEV:
            //  本来应该是有一个cdev的数组,
            //  我们暂时定死一个
            return cdev_write(proc->upgtbl, proc->context.a1, proc->context.a2);
            break;
            
        default:
            break;
    }
    return -1;
}
