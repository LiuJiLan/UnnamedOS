include ../common/common.mk

KERNEL_ASM = \
	entry.S \

KERNEL_C = \
	main.c \
	string.c \
	kalloc.c \
	console.c \
	vm.c \
	sbi.c \
	
KERNEL_PY = \
	test.py \

OBJS = $(KERNEL_ASM:.S=.o)
OBJS += $(KERNEL_C:.c=.o)
OBJS += $(KERNEL_PY:.py=.o)

TRUSHS = $(KERNEL_PY:.py=.S)

.DEFAULT_GOAL := all
all: kernel.elf

kernel.elf: ${OBJS}
	@${CC} ${CFLAGS} -T kernel.ld -o kernel.elf $^
	@${OBJCOPY} -O binary kernel.elf kernel.bin
	@cp kernel.elf ../out/kernel.elf
	@cp kernel.bin ../out/kernel.bin

%.o : %.c
	@${CC} ${CFLAGS} -c -o $@ $<

%.o : %.S
	@${CC} ${CFLAGS} -c -o $@ $<
	
%.S : %.py
	@${PYTHON} $< > $@

QFLAGS += -device loader,file=./testfile/temp_sbi.bin,addr=0x80000000
QFLAGS += -device loader,file=./kernel.bin,addr=0x80200000

.PHONY : debug
debug: all
	@$(OBJDUMP) -D -b binary -m riscv kernel.bin > dis.asm
	@$(OBJDUMP) -S kernel.elf > kernel.asm
	@riscv64-unknown-elf-readelf -a -W kernel.elf > kernel.txt
	@${QEMU} ${QFLAGS} -s -S &
	@${GDB} kernel.elf -q -x ./testfile/gdbinit
	
.PHONY : run
run: all
	@riscv64-unknown-elf-readelf -a -W kernel.elf > kernel.txt
	@${QEMU} ${QFLAGS} -s -S &
	@${GDB} kernel.elf -q -x ./testfile/gdbinit

clean:
	@echo "Check if there is any trush left."
	@echo "Remember to delete the files in ../out ."
	@rm *.o *.bin *.asm *.elf *.txt $(TRUSHS)
