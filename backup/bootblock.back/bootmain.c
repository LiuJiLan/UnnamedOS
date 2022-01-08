//
//  bootmain.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/6.
//

#define ELF_BASE_ADDR   0x80201000U    //  qemu boot rom + ???? (通过dd指定)
//  后续应该找到一个优雅的方式来设置这个值
//  一开始设的值是0x80000000 + ????, 然后一直找不到值
//  突然想起来最高位是1, 有可能做了符号扩展, 所以要标明是U

#include "elf.h"

void todebug(void * p) {
    unsigned long a = (unsigned long) p;
}

void readflash(unsigned char *, unsigned long, unsigned char *);
//  模仿readseg函数
//  由于是64位系统, 函数的第二个参数长度与xv6有所不同
//  由于xv6中是相对于扇区, 我自己的第三个参数是直接传入的ROM/flash中要复制的起点位置

void fillzero(unsigned char *, unsigned long);
//  用于代替xv6中的stosb()函数

void bootmain(void) {
    volatile struct elfhdr * elf;
    struct proghdr *ph, *eph;   // 程序头表
    void (* entry)(void);
    unsigned char * pa;
    
    elf = (struct elfhdr *) ELF_BASE_ADDR;
    
    //  elf = (struct elfhdr *) 0x10000;
    //  xv6中是内存中的0x10000, 因为需要不停地从磁盘复制到内存
    //  在我们的目标平台上, qemu的ROM和k210的flash都是本身就是通过内存进行映射, 所以函数有所不同
    //  此处的地址可以通过ROM/FLASH_BASE_ADDR + dd 命令中指定复制kernel复制到相对于BASE_ADDR的位置来确定
    
    if (elf->magic != ELF_MAGIC) {
        return;
    }
    
    ph = (struct proghdr *)((unsigned char *)elf + elf->phoff);
    eph = ph + elf->phnum;
    for (; ph < eph; ph++) {
        //  for函数有点不那么优雅, 后续版本中改一改, 暂时尽量与xv6保持一致
        //  proghdr中的offset也是相对于elf文件起始地址的偏移
        //  filesz是在elf文件中的长度, memsz是在内存中的实际长度, memsz大于filesz的部分填0
        pa = (unsigned char *)ph->paddr;
        readflash(pa, ph->filesz, (unsigned char *)elf + ph->off);
        
        if (ph->memsz > ph->filesz) {
            fillzero(pa + ph->filesz, ph->memsz - ph->filesz);
        }
    }
    
    entry = (void(*)(void))(elf->entry);
    entry();
}


void readflash(unsigned char * pa, unsigned long count, unsigned char * start) {
    unsigned char * epa;    //  复制目标的结尾边界
    
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
