//
//  bootasm.S
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/6.
//

#define BOOT_STACK_TOP 0x803FFFFFU

.section .text
.globl start
start:
    csrr    a0, mhartid // 获取hart id
    mv      tp, a0
    csrw    mie, zero   // 关中断
    
    bnez    tp, not_start_hart
    //xv6中默认只有first CPU参与了启动, risc-v我们不严谨的认为所有hart都在跑初始代码

    li      sp, BOOT_STACK_TOP
    //xv6中将栈顶指向了0x7c00, 用前面的内存空间做栈
    //在u-boot/include/configs/sipeed-maix.h中CONFIG_SYS_INIT_SP_ADDR = 0x803FFFFF
    //并将这个值与上-16进行4对齐, 最终得到0x803ffff0
    call    bootmain

    //bootmain理应不返回
    //如果想调试可以参考xv6做一个断点
    //此处先死循环
spin:
    j       spin

not_start_hart:
    wfi
    //暂未实现, 根据手册wfi后应跟随一定的判断, 等待ipi的唤醒
    j       not_start_hart

