//
//  sbi_entry.S
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/4/18.
//

//  我们暂不用param这样的文件来定义一些数据, 只是为了快点写出来
//  其实我个人觉得可以直接让M态和S态紧密的贴合在一起,
//  但是我们暂时还是把栈放在能在的最高地址
//  只是担心比赛有何要求, 如果是我自己写就写一块了
//  (就如同实模式转到保护模式那样紧密的排列就好)

//  如果是类Unix系统, 应该接受a0和a1中的变量
//  分别是hartid(可以通过mhartid读取)和dtb的地址
//  分别保存到tp和s1中(s0作为栈帧指针, 所以用s1保存)

.section .text

.globl start
start:
    // 存储mhartid
    csrr    tp, mhartid
    
    //  此处用mie关中断
    csrw    mie, zero

    li      t0, 0x801ffff0
    //  (0x8020_0000 - 1) & -16 4位对齐
    //  第0 hart的栈顶
    li      t1, 1024    //  栈大小
    mul     t1, tp, t1
    sub     sp, t0, t1  //  计算出每个hart的栈顶
    
    //  提前给mscratch写入了
    //  sp只在切换到S态之前为C函数提供栈
    //  在切换过后这之前的sp就不再管理了
    csrw    mscratch, sp

    //  转跳到C语言主程序
    //call    sbimain
