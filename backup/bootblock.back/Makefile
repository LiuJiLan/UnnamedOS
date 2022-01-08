include ./common_back.mk

BOOTBLOCK_BASE_ADDR = 0x00001000	#	boot ROM, provided by qemu

bootblock: bootasm.S bootmain.c
	$(CC) $(CFLAG) -c bootmain.c
	$(CC) $(CFLAG) -c bootasm.S
	$(CC) $(CFLAG) -Ttext=$(BOOTBLOCK_BASE_ADDR) -o bootblock.o bootmain.o bootasm.o
	#xv6中是LD, 暂时偷懒没仔细写Makefile
	#这一段也是大体参考xv6
	$(OBJDUMP) -S bootblock.o > bootblock.asm
	$(OBJCOPY) -S -O binary -j .text bootblock.o bootblock
	
# 自己改写一下
# 注意把common中的QFLAG注释一下
#QFLAGS = -nographic -smp 1 -M virt -m 6M -bios testbios.bin
QFLAGS = -smp 1 -M virt -m 6M
QFLAGS += -bios none
#QFLAGS += -device loader,file=./testbios.bin,addr=0x00001000 # qemu给的是ROM, 不能开栈
QFLAGS += -device loader,file=./testfile/firstjump.bin,addr=0x80000000
QFLAGS += -device loader,file=./testbios.bin,addr=0x80200000
#QFLAGS += -nographic

bootblock.bin: bootasm.S bootmain.c
	$(CC) $(CFLAG) -c bootmain.c -o bootmain.o
	$(CC) $(CFLAG) -c bootasm.S -o bootasm.o
	$(LD) -T ./testfile/test.ld -o bootblock.o bootasm.o bootmain.o
	#$(CC) $(CFLAG) -T ./testfile/test.ld -c -o bootblock.o bootasm.o bootmain.o
	#暂时用bootasm.o在bootmain.o之前来使start处于第一位置
	$(OBJDUMP) -S bootblock.o > bootblock.asm
	$(OBJCOPY) -S -O binary -j .text bootblock.o bootblock.bin
	#$(OBJCOPY) -S -O binary bootblock.o bootblock.bin
	
testbios.bin: bootblock.bin
	dd if=/dev/zero of=testbios.bin bs=1 count=4096
	dd if=bootblock.bin of=testbios.bin conv=notrunc
	dd if=./testfile/test.elf of=testbios.bin bs=1 seek=4096 conv=notrunc
	
test: testbios.bin
	${QEMU} ${QFLAGS} -s -S
	
debug: testbios.bin
	$(OBJDUMP) -D -b binary -m riscv  bootblock.bin > asm.asm
	${GDB} bootblock.o -q -x ./testfile/gdbinit

debug2: testbios.bin
	${QEMU} ${QFLAGS} -s -S &
	$(OBJDUMP) -D -b binary -m riscv  bootblock.bin > asm.asm
	${GDB} bootblock.o -q -x ./testfile/gdbinit
	
debug3:
	${QEMU} ${QFLAGS} -s -S &
	$(OBJDUMP) -D -b binary -m riscv  bootblock.bin > asm.asm
	${GDB} bootblock.o -q -x ./testfile/gdbinit
	
clean:
	rm *.o *.bin *.asm
