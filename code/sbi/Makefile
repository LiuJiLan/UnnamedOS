include ../common/common.mk

KERNEL_ASM = \
	sbi_asm.S \

KERNEL_C = \
	sbi_main.c \
	loader.c \
	
KERNEL_PY = \
	kernel_elf.py \

OBJS = $(KERNEL_ASM:.S=.o)
OBJS += $(KERNEL_C:.c=.o)
OBJS += $(KERNEL_PY:.py=.o)

TRUSHS = $(KERNEL_PY:.py=.S)

.DEFAULT_GOAL := all
all: sbi.elf

sbi.elf: ${OBJS}
	@${CC} ${CFLAGS} -T sbi.ld -o sbi.elf $^
	@${OBJCOPY} -O binary sbi.elf sbi.bin
	
kernel_elf.S: kernel_elf.py
	@cd ../kernel; make kernel.elf -B
	@${PYTHON} kernel_elf.py > kernel_elf.S

%.o : %.c
	@${CC} ${CFLAGS} -c -o $@ $<

%.o : %.S
	@${CC} ${CFLAGS} -c -o $@ $<
	
%.S : %.py
	@${PYTHON} $< > $@

QFLAGS += -device loader,file=./sbi.bin,addr=0x80000000

.PHONY : debug
debug: all
	@$(OBJDUMP) -D -b binary -m riscv sbi.bin > dis.asm
	@$(OBJDUMP) -S sbi.elf > sbi.asm
	@riscv64-unknown-elf-readelf -a -W sbi.elf > sbi.txt
	@${QEMU} ${QFLAGS} -s -S &
	@${GDB} sbi.elf -q -x ./testfile/gdbinit
	
.PHONY : run
run: all
	@riscv64-unknown-elf-readelf -a -W sbi.elf > sbi.txt
	@${QEMU} ${QFLAGS} -s -S &
	@${GDB} ./sbi.elf ../out/kernel.elf -q -x ./testfile/gdbinit

clean:
	@rm *.o *.bin *.asm *.elf *.txt $(TRUSHS)
	@cd ../kernel; make clean

