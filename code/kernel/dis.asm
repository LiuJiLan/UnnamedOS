
kernel.bin:     file format binary


Disassembly of section .data:

0000000000000000 <.data>:
       0:	52fd                	li	t0,-1
       2:	3b029073          	csrw	pmpaddr0,t0
       6:	42bd                	li	t0,15
       8:	3a02a073          	csrs	pmpcfg0,t0
       c:	6285                	lui	t0,0x1
       e:	8002829b          	0x8002829b
      12:	3002a073          	csrs	mstatus,t0
      16:	00000297          	auipc	t0,0x0
      1a:	01028293          	addi	t0,t0,16 # 0x26
      1e:	34129073          	csrw	mepc,t0
      22:	30200073          	mret
      26:	00001297          	auipc	t0,0x1
      2a:	fda28293          	addi	t0,t0,-38 # 0x1000
      2e:	8e16                	mv	t3,t0
      30:	a010031b          	0xa010031b
      34:	0356                	slli	t1,t1,0x15
      36:	406282b3          	sub	t0,t0,t1
      3a:	00c2d293          	srli	t0,t0,0xc
      3e:	fff0031b          	0xfff0031b
      42:	137e                	slli	t1,t1,0x3f
      44:	0062e2b3          	or	t0,t0,t1
      48:	18029073          	csrw	satp,t0
      4c:	12000073          	sfence.vma
      50:	00003117          	auipc	sp,0x3
      54:	fb010113          	addi	sp,sp,-80 # 0x3000
      58:	00000297          	auipc	t0,0x0
      5c:	00a28293          	addi	t0,t0,10 # 0x62
      60:	8282                	jr	t0
      62:	1141                	addi	sp,sp,-16
      64:	e422                	fsw	fs0,8(sp)
      66:	0800                	addi	s0,sp,16
      68:	a001                	j	0x68
	...
    100e:	0000                	unimp
    1010:	200000cf          	fnmadd.s	ft1,ft0,ft0,ft4,rne
	...
    1ff8:	200000cf          	fnmadd.s	ft1,ft0,ft0,ft4,rne
    1ffc:	0000                	unimp
	...
