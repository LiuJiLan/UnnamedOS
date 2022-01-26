//
//  sbi_type.h
//  xv6-qemu
//
//  Created by 刘冬辰 on 2022/1/22.
//

#ifndef sbi_type_h
#define sbi_type_h

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

typedef uptr_t           pte_t;

#endif /* sbi_type_h */
