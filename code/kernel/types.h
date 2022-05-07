//
//  types.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/4/25.
//

#ifndef types_h
#define types_h

#define NULL ((void*) 0)

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
typedef unsigned long       size_t;


#define PGSIZE      4096
#define PGSHIFT     12

#define PGROUNDUP(sz)  (((sz)+PGSIZE-1) & ~(PGSIZE-1))
#define PGROUNDDOWN(a) (((a)) & ~(PGSIZE-1))


#endif /* types_h */
