//
//  main.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/12.
//

#include "mmu.h"

extern pte_t entrypgdir[];

int main(void) {
    while (1) {
    }
}



__attribute__((__aligned__(PGSIZE)))
extern pte_t entrypgdir[NPTE1] = {
    //  注意页表项的高位新增了一些用于新增指令集的位, 我们暂时设0
    [2] = (0x80000 << 10) | 0xcf,
    //  我们没有像xv6一样使用mmu.h中的宏, 因为只是临时页表
    [511] = (0x80000 << 10) | 0xcf
};
