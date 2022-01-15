//
//  entry.S
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/9.
//

#include "memlayout.h"
//#include "mmu.h"
//  xv6中"mmu.h"存储了部分有关建立页表所需的宏定义
//  例如分页模式之类的参数
//  第二版再来讨论是否需要把这些东西放入头文件中
#include "param.h"

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

    //  注意大页模式下的一些问题
    //  之前由于想完全参考rCore, 所以把内核的虚拟地址设置在0xFFFFFFFFC0200000U
    //  这是一个致命的错误:
    //  因为我的内核所在的物理地址是0x80000000, 而rCore内核的物理地址是在0x80200000
    //  (那0x0020_0000用于SBI)
    //  我试图在大页模式下将0xFFFFFFFFC0200000U映射到0x80000000, 这是无法实现的
    //  一级大页下的效果的本质是把0xFFFFFFFFC映射到了0x000000008
    //  后面的部分都是偏移量, 我的错误映射会导致一个0x0020_0000的偏差无法补上
    //  由于已经有了二周目补SBI的想法, 我们这里就不做复杂的多级映射去补这个0x0020_0000了
    //  所以将memlayout.h中的KERNBASE做了更改

    //  想开页表先进S-Mode
    //  (因为M-Mode下是否使用虚拟地址转换是不确定的)
    //  如果有PMP系统需要先配置, 不然mret之后会有错误
    //  x86体系中内核有着最高权限, risc-v中内核上还有M-Mode
    //  这也是为什么需要SBI
    //  Note: 进S-Mode在实际应该在SBI启动的末尾完成, 我们这里只是初期设计, 放入代办事项
    
    //  设置PMP
    li      t0, -1          //  0b111...111
    csrw    pmpaddr0, t0    //  54-63位是WARL, 所以高位随便写(含义见特权手册)
    
    li      t0, 0x0f        //  A=0x01 TOR, X=W=R=1
    csrs    pmpcfg0, t0
    
    //  进S-Mode
    li      t0, 0x1 << 11   //  mstatus.MPP = 0b01 S Mode
    csrs    mstatus, t0

    la      t0, entry_s_mode
    csrw    mepc, t0        //  为什么这么做参考手册中mret指令

    mret

entry_s_mode:               //  标志着进入了S-Mode
    //  设置页表
    //  开始设置satp寄存器
    
    //  li      t0, (V2P_WO(entrypgdir))
    //  用la报offset too large
    //  用li报illegal operands
    
    //  v2p_entrypgdir = V2P_WO(entrypgdir)
    //  la      t0, v2p_entrypgdir
    //  elocation truncated to fit: R_RISCV_PCREL_HI20 against symbol `entrypgdir' defined in .data section in main.o
    //  li      t0, v2p_entrypgdir
    //  illegal operands

    //  只能自己乖乖算了
    //  但是也不能用la加载, 也不能用li加载, 详见README
    //  la      t0, entrypgdir
    //  li      t1, V_P_DIFF
    //  sub     t0, t0, t1

    //  这样也会出错
    //  lui     t0, %hi(V2P_WO(entrypgdir))
    //  addi    t0, t0, %lo(V2P_WO(entrypgdir))

    lui     t0, %hi(entrypgdir)
    addi    t0, t0, %lo(entrypgdir)
    //  注意这个方法加载的其实是符号位扩展的32位地址
    li      t1, V_P_DIFF
    sub     t0, t0, t1

    srli    t0, t0, 12      //  右移, 空位填0

    li      t1, 0x8 << 60   //  8代表Sv39分页模式
    or      t0, t0, t1

    csrw    satp, t0        //  启动分页
    sfence.vma              //  刷新快表

    
    //  la      sp, stack + KSTACKSIZE  //  "param.h"中指定
    lui     sp, %hi(stack + KSTACKSIZE)
    addi    sp, sp, %lo(stack + KSTACKSIZE)

    //  la      t0, main        //  进kernel的main函数
    lui     t0, %hi(main)
    addi    t0, t0, %lo(main)
    jr      t0

.comm stack, KSTACKSIZE
