//
//  entry.S
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/9.
//

//#include "asm.h"
#include "memlayout.h"
//#include "mmu.h"
//#include "param.h"

//  _start是给bootblock使用用来加载elf文件的程序入口
//  entry使用的是链接脚本中的虚拟地址
//  所以链接脚本中入口被设为_start, 而后续的摆放则是由虚拟地址开始摆放

.globl _start
_start = V2P_WO(entry)

//  V2P_WO宏定义在memlayout.h
//  memlayout.h中的部分变量是暂定的, 并且要和链接脚本相呼应

.globl entry
entry:
    //  xv6中此处只开启了大页模式, 推测是出于编程简单的原因
    //  在此也暂时保持和xv6保持一致, 暂时只采用一级页表映射

    //  此时虚拟内存并没有完全精准的映射, 相当于30-38位相同的合法虚拟地址都会映射到统一物理地址
    //
