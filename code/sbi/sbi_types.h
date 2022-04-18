//
//  sbi_types.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/4/18.
//

#ifndef sbi_types_h
#define sbi_types_h

typedef char                int8;
typedef unsigned char       uint8;
typedef short               int16;
typedef unsigned short      uint16;
typedef int                 int32;
typedef unsigned int        uint32;
typedef long long           int64;
typedef unsigned long long  uint64;

//  平台字长相关(void * 的长度)
typedef long                ptr_t;
typedef unsigned long       uptr_t;

typedef unsigned long       regs_t;

#endif /* sbi_types_h */
