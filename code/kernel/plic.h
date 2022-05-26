//
//  plic.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/5/26.
//

#ifndef plic_h
#define plic_h

#define PLIC_BASE 0x0C000000L
#define PLIC_SIZE 0x400000  //  这个值是从xv6-riscv中得来的, 其实不是正确的值

void plic_init(void);
void plic_init_hart(void);
int plic_claim(void);
void plic_complete(int);

#endif /* plic_h */
