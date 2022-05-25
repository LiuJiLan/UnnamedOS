//
//  pipe.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/5/25.
//

//  pipe应该有多个实例, 但是我们暂时只有一个实例

#ifndef pipe_h
#define pipe_h

#include "spinlock.h"
#include "types.h"
#include "vm.h"

#define PIPE_SZ 64

struct pipe {
    int head;
    int tail;
    char data[PIPE_SZ];
    //  由于进程fork的关系,
    //  可能会有多个进程对同对同一个pipe实例进行同一种操作的修改
    struct spinlock lock;
};

//  本来应该是有一个pipe设备的表
//  每次需要用pipe的时候去申请,
//  但是我们现在的方法是直接定死一个
struct pipe pipedev;

ssize_t pipe_write(struct pipe * pi, pgtbl_t upgtbl, uptr_t uva, size_t count);
ssize_t pipe_read(struct pipe * pi, pgtbl_t upgtbl, uptr_t uva, size_t count);


#endif /* pipe_h */
