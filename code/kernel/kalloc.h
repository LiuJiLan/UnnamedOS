//
//  kalloc.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/4/24.
//

#ifndef kalloc_h
#define kalloc_h

//注意, 两者的输入与返回值都是虚拟地址

void kalloc_init(void);
char * kalloc(void);
void kfree(char *);

//  kfree的输入要求
//  (1)与页对齐
//  (2)在内核结尾之后
//  (3)在回收的内存应该位于最高物理内存内

#endif /* kalloc_h */
