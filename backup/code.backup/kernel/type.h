//
//  type.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/19.
//

//  我们对types.h的态度是
//  (没错, 我们第一版把types写错成type了)
//  原则上不去额外定义C固有类型的简写
//  例如:unsigned char我们不重命名为uchar

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
typedef long                ptr_t;
typedef unsigned long       uptr_t;

typedef unsigned long       regs_t;
typedef unsigned long       size_t;

typedef uptr_t           pte_t;

typedef pte_t pde_t;
//  由于我们后面考虑重命名这个type, 所以我们在这里重新加上
//  前面部分本是pde的部分被我们改成了pte, 这次更改后我们不再更改

#endif /* type_h */
