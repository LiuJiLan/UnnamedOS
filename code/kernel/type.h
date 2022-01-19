//
//  type.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/19.
//

#ifndef type_h
#define type_h

//  平台字长无关
typedef char                int8;
typedef unsigned char       uint8;
typedef short               int16;
typedef unsigned short      uint16;
typedef int                 int32;
typedef unsigned int        uint32;
typedef long long           int64;
typedef unsigned long long  uint64;

//  平台字长相关(void * 的长度)
typedef long                intptr_t;
typedef unsigned long       uintptr_t;

typedef uintptr_t           pte_t;

#endif /* type_h */
