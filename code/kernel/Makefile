include ../common/common.mk

KERNEL_ASM = \
	entry.S \
	trap_handler.S \
	initcode.S \

KERNEL_C = \
	kmain.c \
	trap.c \
	hart.c \
	intr.c \
	spinlock.c \
	kalloc.c \
	vm.c \
	proc.c \
	fd.c \
	sleeplock.c \
	syscall.c \
	temp.c \
	file.c \
	time.c \
	ccache.c \
	uart.c \
	string.c \
	
KERNEL_PY = \
	

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

QFLAGS += -device loader,file=../out/sbi.bin,addr=0x80000000
QFLAGS += -device loader,file=./kernel.bin,addr=0x80200000

sbi:
	@cd ../sbi; make sbi.elf -B

.PHONY : debug
debug: all
	@$(OBJDUMP) -D -b binary -m riscv kernel.bin > dis.asm
	@$(OBJDUMP) -S kernel.elf > kernel.asm
	@$(READELF) -a -W kernel.elf > kernel.txt
	@${QEMU} ${QFLAGS} -s -S &
	@${GDB} kernel.elf -q -x ./testfile/gdbinit
	
.PHONY : run
run: all
	@$(READELF) -a -W kernel.elf > kernel.txt
	@${QEMU} ${QFLAGS} -s -S &
	@${GDB} kernel.elf -q -x ./testfile/gdbinit

clean:
	@echo "Check if there is any trush left."
	@echo "Remember to delete the files in ../out ."
	@rm *.o *.bin *.asm *.elf *.txt $(TRUSHS)

initcode.S:
	# initcode.S必须确保是最新的
	@python3 ../../script/init_proc.py
