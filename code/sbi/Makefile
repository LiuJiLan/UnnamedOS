include ../common/common.mk

KERNEL_ASM = \
	sbi_entry.S \
	sbi_trap_handler.S \

KERNEL_C = \
	sbi_main.c \
	clint.c \
	sbi_trap.c \
	
KERNEL_PY = \
	#kernel_elf.py \

OBJS = $(KERNEL_ASM:.S=.o)
OBJS += $(KERNEL_C:.c=.o)
OBJS += $(KERNEL_PY:.py=.o)

TRUSHS = $(KERNEL_PY:.py=.S)

.DEFAULT_GOAL := all
all: sbi.elf

sbi.elf: ${OBJS}
	@${CC} ${CFLAGS} -T sbi.ld -o sbi.elf $^
	@${OBJCOPY} -O binary sbi.elf sbi.bin
	@cp sbi.elf ../out/sbi.elf
	@cp sbi.bin ../out/sbi.bin
	
kernel_elf.S: kernel_elf.py
	@cd ../kernel; make kernel.elf -B
	@${PYTHON} kernel_elf.py > kernel_elf.S

%.o : %.c
	@${CC} ${CFLAGS} -c -o $@ $<

%.o : %.S
	@${CC} ${CFLAGS} -c -o $@ $<
	
%.S : %.py
	@${PYTHON} $< > $@

QFLAGS += -device loader,file=../out/sbi.bin,addr=0x80000000
QFLAGS += -device loader,file=../out/kernel.bin,addr=0x80200000

kernel:
	@cd ../kernel; make kernel.elf -B

.PHONY : debug
debug: all  kernel
	@$(OBJDUMP) -D -b binary -m riscv sbi.bin > dis.asm
	@$(OBJDUMP) -S sbi.elf > sbi.asm
	@$(READELF) -a -W sbi.elf > sbi.txt
	@${QEMU} ${QFLAGS} -s -S &
	@${GDB} sbi.elf -q -x ./testfile/gdbinit
	
.PHONY : run
run: all
	@$(READELF) -a -W sbi.elf > sbi.txt
	@${QEMU} ${QFLAGS} -s -S &
	@${GDB} sbi.elf ../out/kernel.elf -q -x ./testfile/gdbinit

clean:
	@rm *.o *.bin *.asm *.elf *.txt $(TRUSHS)
	@cd ../kernel; make clean

