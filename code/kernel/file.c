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
    //  BUG!!!
    //  我们暂时用文件的FLAGS位来保证同一个cdev的读写之分
    //  我们一开始把init进程STDIN、STDOUT、STDERR都指向了这个CDEV
    //  其实ref的增加应该由进程部分的函数来处理
    kftable.filetbl[0].ref = 3;
    kftable.filetbl[0].usable = F_UNUSABLE;
    
    kftable.filetbl[1].type = PIPE;
    kftable.filetbl[1].pipe_p = &pipedev;
    kftable.filetbl[1].ref = 2;
    kftable.filetbl[1].usable = F_UNUSABLE;
}

void file_increase(struct file * f) {
    acquire(&kftable.lock);
    f->ref++;
    release(&kftable.lock);
}

void file_decrease(struct file * f) {
    acquire(&kftable.lock);
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
            
        case PIPE:
            //  本来应该是有一个cdev的数组,
            //  我们暂时定死一个
            return pipe_read(f->pipe_p, proc->upgtbl, proc->context.a1, proc->context.a2);
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
            
        case PIPE:
            //  本来应该是有一个cdev的数组,
            //  我们暂时定死一个
            return pipe_write(f->pipe_p, proc->upgtbl, proc->context.a1, proc->context.a2);
            break;
            
        default:
            break;
    }
    return -1;
}
