
bootblock.o:     file format elf64-littleriscv


Disassembly of section .text:

0000000000001000 <bootmain>:
    1000:	7139                	addi	sp,sp,-64
    1002:	fc06                	sd	ra,56(sp)
    1004:	f822                	sd	s0,48(sp)
    1006:	0080                	addi	s0,sp,64
    1008:	67c5                	lui	a5,0x11
    100a:	fef43023          	sd	a5,-32(s0)
    100e:	fe043783          	ld	a5,-32(s0)
    1012:	439c                	lw	a5,0(a5)
    1014:	873e                	mv	a4,a5
    1016:	464c47b7          	lui	a5,0x464c4
    101a:	57f78793          	addi	a5,a5,1407 # 464c457f <boot_stack+0x464c33d5>
    101e:	0af71f63          	bne	a4,a5,10dc <bootmain+0xdc>
    1022:	fe043783          	ld	a5,-32(s0)
    1026:	739c                	ld	a5,32(a5)
    1028:	fe043703          	ld	a4,-32(s0)
    102c:	97ba                	add	a5,a5,a4
    102e:	fef43423          	sd	a5,-24(s0)
    1032:	fe043783          	ld	a5,-32(s0)
    1036:	0387d783          	lhu	a5,56(a5)
    103a:	873e                	mv	a4,a5
    103c:	87ba                	mv	a5,a4
    103e:	078e                	slli	a5,a5,0x3
    1040:	8f99                	sub	a5,a5,a4
    1042:	078e                	slli	a5,a5,0x3
    1044:	873e                	mv	a4,a5
    1046:	fe843783          	ld	a5,-24(s0)
    104a:	97ba                	add	a5,a5,a4
    104c:	fcf43c23          	sd	a5,-40(s0)
    1050:	a0bd                	j	10be <bootmain+0xbe>
    1052:	fe843783          	ld	a5,-24(s0)
    1056:	6f9c                	ld	a5,24(a5)
    1058:	fcf43423          	sd	a5,-56(s0)
    105c:	fe843783          	ld	a5,-24(s0)
    1060:	7394                	ld	a3,32(a5)
    1062:	fe843783          	ld	a5,-24(s0)
    1066:	679c                	ld	a5,8(a5)
    1068:	079a                	slli	a5,a5,0x6
    106a:	fe043703          	ld	a4,-32(s0)
    106e:	97ba                	add	a5,a5,a4
    1070:	863e                	mv	a2,a5
    1072:	85b6                	mv	a1,a3
    1074:	fc843503          	ld	a0,-56(s0)
    1078:	06e000ef          	jal	ra,10e6 <readflash>
    107c:	fe843783          	ld	a5,-24(s0)
    1080:	7798                	ld	a4,40(a5)
    1082:	fe843783          	ld	a5,-24(s0)
    1086:	739c                	ld	a5,32(a5)
    1088:	02e7f563          	bgeu	a5,a4,10b2 <bootmain+0xb2>
    108c:	fe843783          	ld	a5,-24(s0)
    1090:	739c                	ld	a5,32(a5)
    1092:	fc843703          	ld	a4,-56(s0)
    1096:	00f706b3          	add	a3,a4,a5
    109a:	fe843783          	ld	a5,-24(s0)
    109e:	7798                	ld	a4,40(a5)
    10a0:	fe843783          	ld	a5,-24(s0)
    10a4:	739c                	ld	a5,32(a5)
    10a6:	40f707b3          	sub	a5,a4,a5
    10aa:	85be                	mv	a1,a5
    10ac:	8536                	mv	a0,a3
    10ae:	094000ef          	jal	ra,1142 <fillzero>
    10b2:	fe843783          	ld	a5,-24(s0)
    10b6:	03878793          	addi	a5,a5,56
    10ba:	fef43423          	sd	a5,-24(s0)
    10be:	fe843703          	ld	a4,-24(s0)
    10c2:	fd843783          	ld	a5,-40(s0)
    10c6:	f8f766e3          	bltu	a4,a5,1052 <bootmain+0x52>
    10ca:	fe043783          	ld	a5,-32(s0)
    10ce:	6f9c                	ld	a5,24(a5)
    10d0:	fcf43823          	sd	a5,-48(s0)
    10d4:	fd043783          	ld	a5,-48(s0)
    10d8:	9782                	jalr	a5
    10da:	a011                	j	10de <bootmain+0xde>
    10dc:	0001                	nop
    10de:	70e2                	ld	ra,56(sp)
    10e0:	7442                	ld	s0,48(sp)
    10e2:	6121                	addi	sp,sp,64
    10e4:	8082                	ret

00000000000010e6 <readflash>:
    10e6:	7139                	addi	sp,sp,-64
    10e8:	fc22                	sd	s0,56(sp)
    10ea:	0080                	addi	s0,sp,64
    10ec:	fca43c23          	sd	a0,-40(s0)
    10f0:	fcb43823          	sd	a1,-48(s0)
    10f4:	fcc43423          	sd	a2,-56(s0)
    10f8:	fd843703          	ld	a4,-40(s0)
    10fc:	fd043783          	ld	a5,-48(s0)
    1100:	97ba                	add	a5,a5,a4
    1102:	fef43423          	sd	a5,-24(s0)
    1106:	a01d                	j	112c <readflash+0x46>
    1108:	fc843783          	ld	a5,-56(s0)
    110c:	0007c703          	lbu	a4,0(a5)
    1110:	fd843783          	ld	a5,-40(s0)
    1114:	00e78023          	sb	a4,0(a5)
    1118:	fd843783          	ld	a5,-40(s0)
    111c:	0785                	addi	a5,a5,1
    111e:	fcf43c23          	sd	a5,-40(s0)
    1122:	fc843783          	ld	a5,-56(s0)
    1126:	0785                	addi	a5,a5,1
    1128:	fcf43423          	sd	a5,-56(s0)
    112c:	fd843703          	ld	a4,-40(s0)
    1130:	fe843783          	ld	a5,-24(s0)
    1134:	fcf76ae3          	bltu	a4,a5,1108 <readflash+0x22>
    1138:	0001                	nop
    113a:	0001                	nop
    113c:	7462                	ld	s0,56(sp)
    113e:	6121                	addi	sp,sp,64
    1140:	8082                	ret

0000000000001142 <fillzero>:
    1142:	7179                	addi	sp,sp,-48
    1144:	f422                	sd	s0,40(sp)
    1146:	1800                	addi	s0,sp,48
    1148:	fca43c23          	sd	a0,-40(s0)
    114c:	fcb43823          	sd	a1,-48(s0)
    1150:	fd843703          	ld	a4,-40(s0)
    1154:	fd043783          	ld	a5,-48(s0)
    1158:	97ba                	add	a5,a5,a4
    115a:	fef43423          	sd	a5,-24(s0)
    115e:	a811                	j	1172 <fillzero+0x30>
    1160:	fd843783          	ld	a5,-40(s0)
    1164:	00078023          	sb	zero,0(a5)
    1168:	fd843783          	ld	a5,-40(s0)
    116c:	0785                	addi	a5,a5,1
    116e:	fcf43c23          	sd	a5,-40(s0)
    1172:	fd843703          	ld	a4,-40(s0)
    1176:	fe843783          	ld	a5,-24(s0)
    117a:	fef763e3          	bltu	a4,a5,1160 <fillzero+0x1e>
    117e:	0001                	nop
    1180:	0001                	nop
    1182:	7422                	ld	s0,40(sp)
    1184:	6145                	addi	sp,sp,48
    1186:	8082                	ret

0000000000001188 <start>:
    1188:	f1402573          	csrr	a0,mhartid
    118c:	822a                	mv	tp,a0
    118e:	30401073          	csrw	mie,zero
    1192:	00021963          	bnez	tp,11a4 <not_start_hart>
    1196:	00000117          	auipc	sp,0x0
    119a:	01410113          	addi	sp,sp,20 # 11aa <boot_stack>
    119e:	e63ff0ef          	jal	ra,1000 <bootmain>

00000000000011a2 <spin>:
    11a2:	a001                	j	11a2 <spin>

00000000000011a4 <not_start_hart>:
    11a4:	10500073          	wfi
    11a8:	bff5                	j	11a4 <not_start_hart>

00000000000011aa <boot_stack>:
	...
