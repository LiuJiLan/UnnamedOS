//
//  memlayout.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/4/22.
//

#ifndef memlayout_h
#define memlayout_h

#define PHYSTOP 0x80600000U

#define V_P_DIFF 0xFFFFFFFF40000000U

#define V2P(a) (((unsigned long) (a)) - V_P_DIFF)   //  64位下改了数据类型
#define P2V(a) ((void *)(((unsigned char *) (a)) + V_P_DIFF))

//  纯数值的转换
#define V2P_WO(x) ((x) - V_P_DIFF)    // same as V2P, but without casts //  不带类型转换
#define P2V_WO(x) ((x) + V_P_DIFF)    // same as P2V, but without casts

#endif /* memlayout_h */
