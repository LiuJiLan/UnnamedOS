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
#define NFILE               64      //  打开的文件数量
#define DEFAULT_TIME        16      //  默认时间片
#define DEFAULT_INTERVAL    0x10000 //  默认时钟中断间隔
#define MECHINE_FEQUENCY    44100   //  所使用机器的频率, 临时用来控制时间(单位Hz)

#endif /* param_h */
