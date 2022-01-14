
kernel.elf:     file format elf64-littleriscv


Disassembly of section .text:

ffffffffc0200000 <entry>:
    //  x86体系中内核有着最高权限, risc-v中内核上还有M-Mode
    //  这也是为什么需要SBI
    //  Note: 进S-Mode在实际应该在SBI启动的末尾完成, 我们这里只是初期设计, 放入代办事项
    
    //  设置PMP
    li      t0, -1          //  0b111...111
ffffffffc0200000:	52fd                	li	t0,-1
    csrw    pmpaddr0, t0    //  54-63位是WARL, 所以高位随便写(含义见特权手册)
ffffffffc0200002:	3b029073          	csrw	pmpaddr0,t0
    
    li      t0, 0x0f        //  A=0x01 TOR, X=W=R=1
ffffffffc0200006:	42bd                	li	t0,15
    csrs    pmpcfg0, t0
ffffffffc0200008:	3a02a073          	csrs	pmpcfg0,t0
    
    //  进S-Mode
    li      t0, 0x1 << 11   //  mstatus.MPP = 0b01 S Mode
ffffffffc020000c:	6285                	lui	t0,0x1
ffffffffc020000e:	8002829b          	addiw	t0,t0,-2048
    csrs    mstatus, t0
ffffffffc0200012:	3002a073          	csrs	mstatus,t0

    la      t0, entry_s_mode
ffffffffc0200016:	00000297          	auipc	t0,0x0
ffffffffc020001a:	01028293          	addi	t0,t0,16 # ffffffffc0200026 <entry_s_mode>
    csrw    mepc, t0        //  为什么这么做参考手册中mret指令
ffffffffc020001e:	34129073          	csrw	mepc,t0

    mret
ffffffffc0200022:	30200073          	mret

ffffffffc0200026 <entry_s_mode>:
    //  elocation truncated to fit: R_RISCV_PCREL_HI20 against symbol `entrypgdir' defined in .data section in main.o
    //  li      t0, v2p_entrypgdir
    //  illegal operands

    //  只能自己乖乖算了
    la      t0, entrypgdir
ffffffffc0200026:	00001297          	auipc	t0,0x1
ffffffffc020002a:	fda28293          	addi	t0,t0,-38 # ffffffffc0201000 <entrypgdir>
    mv      t3, t0
ffffffffc020002e:	8e16                	mv	t3,t0
    li      t1, V_P_DIFF
ffffffffc0200030:	a010031b          	addiw	t1,zero,-1535
ffffffffc0200034:	0356                	slli	t1,t1,0x15
    sub     t0, t0, t1
ffffffffc0200036:	406282b3          	sub	t0,t0,t1

    srli    t0, t0, 12      //  右移, 空位填0
ffffffffc020003a:	00c2d293          	srli	t0,t0,0xc

    li      t1, 0x8 << 60   //  8代表Sv39分页模式
ffffffffc020003e:	fff0031b          	addiw	t1,zero,-1
ffffffffc0200042:	137e                	slli	t1,t1,0x3f
    or      t0, t0, t1
ffffffffc0200044:	0062e2b3          	or	t0,t0,t1

    csrw    satp, t0        //  启动分页
ffffffffc0200048:	18029073          	csrw	satp,t0
    sfence.vma              //  刷新快表
ffffffffc020004c:	12000073          	sfence.vma

    
    la      sp, stack + KSTACKSIZE  //  "param.h"中指定
ffffffffc0200050:	00003117          	auipc	sp,0x3
ffffffffc0200054:	fb010113          	addi	sp,sp,-80 # ffffffffc0203000 <stack+0x1000>

    la      t0, main        //  进kernel的main函数
ffffffffc0200058:	00000297          	auipc	t0,0x0
ffffffffc020005c:	00a28293          	addi	t0,t0,10 # ffffffffc0200062 <main>
    jr      t0
ffffffffc0200060:	8282                	jr	t0

ffffffffc0200062 <main>:

#include "mmu.h"

extern pte_t entrypgdir[];

int main(void) {
ffffffffc0200062:	1141                	addi	sp,sp,-16
ffffffffc0200064:	e422                	sd	s0,8(sp)
ffffffffc0200066:	0800                	addi	s0,sp,16
    while (1) {
ffffffffc0200068:	a001                	j	ffffffffc0200068 <main+0x6>
