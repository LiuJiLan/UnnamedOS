//
//  param.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/4/22.
//

#ifndef param_h
#define param_h

#define N_HART              3       //  注意是数量
#define KERNEL_STACK_SIZE   2048    //  内核栈大小
#define NPROC               64      //  进程数量
#define DEFAULT_TIME        32      //  默认时间片
#define DEFAULT_SBI_TIMER   0x10000

#endif /* param_h */
