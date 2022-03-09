include ../common/common.mk

bootblock: bootasm.S bootmain.c
	$(CC) $(CFLAG) -c bootmain.c
	$(CC) $(CFLAG) -c bootasm.S
	$(LD) -T bootblock.ld -o bootblock.o bootasm.o bootmain.o
	$(OBJDUMP) -S bootblock.o > bootblock.asm
	$(OBJCOPY) -S -O binary -j .text bootblock.o bootblock

# 自己改写一下

.DEFAULT_GOAL := debug

QFLAGS = -smp 1 -M virt -m 6M
QFLAGS += -bios none
QFLAGS += -nographic
QFLAGS += -device loader,file=./testbios.bin,addr=0x01000

bootblock.bin: bootasm.S bootmain.c
	$(CC) $(CFLAG) -c bootmain.c -o bootmain.o
	$(CC) $(CFLAG) -c bootasm.S -o bootasm.o
	$(LD) -T ./testfile/test.ld -o bootblock.o bootasm.o bootmain.o
	$(OBJCOPY) -S -O binary -j .text bootblock.o bootblock.bin
	
testbios.bin: bootblock.bin
	dd if=/dev/zero of=testbios.bin bs=1 count=2048
	dd if=bootblock.bin of=testbios.bin conv=notrunc
	dd if=./testfile/test.elf of=testbios.bin bs=1 seek=2048 conv=notrunc
	
test: testbios.bin
	${QEMU} ${QFLAGS} -s -S
	
debug: testbios.bin
	${GDB} bootblock.o -q -x ./testfile/gdbinit
	
clean:
	rm *.o *.bin *.asm
