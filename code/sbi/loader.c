//
//  loader.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/21.
//

#include "../common/elf.h"
#include "sbi_defs.h"

extern char kernel_elf[];   //  跟ld一样只能用这种方式引入

static inline unsigned long get_kernel_elf(void);
void readflash(unsigned char *, unsigned long, unsigned char *);
void fillzero(unsigned char *, unsigned long);

uptr_t loader_kernel(void) {
    struct elfhdr * elf;
    struct proghdr *ph, *eph;   // 程序头表
    // void (* entry)(void);
    
    unsigned char * pa;
    
    elf = (struct elfhdr *)kernel_elf;
    
    if (elf->magic != ELF_MAGIC) {
        return 0;
    }
    
    ph = (struct proghdr *)((unsigned char *)elf + elf->phoff);
    eph = ph + elf->phnum;
    for (; ph < eph; ph++) {
        pa = (unsigned char *)ph->paddr;
        readflash(pa, ph->filesz, (unsigned char *)elf + ph->off);
        if (ph->memsz > ph->filesz) {
            fillzero(pa + ph->filesz, ph->memsz - ph->filesz);
        }
    }
    
    //  panic("FUCK");
    
    //  entry = (void(*)(void))(elf->entry);
    //  entry();
    return elf->entry;
}


void readflash(unsigned char * pa, unsigned long count, unsigned char * start) {
    unsigned char * epa;
    
    epa = pa + count;
    
    for (; pa < epa; pa++, start++) {
        *pa = *start;
    }
}

void fillzero(unsigned char * start, unsigned long count) {
    unsigned char * end = start + count;
    while (start < end) {
        *start = 0x0U;
        start++;
    }
}

//static inline unsigned long get_kernel_elf(void) {
//    unsigned long kernel_elf_base_address;
//    asm volatile("la    %0, kernel_elf;"
//                 :"=r"(kernel_elf_base_address)
//                 ://    无输入
//                 ://    无破损
//                 );
//    return kernel_elf_base_address;
//}
