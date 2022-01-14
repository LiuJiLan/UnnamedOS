include ../common.mk

SRCS_ASM = \
	entry.S \

SRCS_C = \
	main.c \

OBJS = $(SRCS_ASM:.S=.o)
OBJS += $(SRCS_C:.c=.o)

.DEFAULT_GOAL := all
all: kernel.elf

kernel.elf: ${OBJS}
	${CC} ${CFLAGS} -T kernel.ld -o kernel.elf $^
	${OBJCOPY} -O binary kernel.elf kernel.bin

%.o : %.c
	${CC} ${CFLAGS} -c -o $@ $<

%.o : %.S
	${CC} ${CFLAGS} -c -o $@ $<

QFLAGS += -device loader,file=./kernel.bin,addr=0x80000000

.PHONY : debug
debug: all
	@$(OBJDUMP)  -D -b binary -m riscv kernel.bin > dis.asm
	@$(OBJDUMP) -S kernel.elf > kernel.asm
	@riscv64-unknown-elf-readelf -a -W kernel.elf > kernel.txt
	@${QEMU} ${QFLAGS} -s -S &
	@${GDB} kernel.elf -q -x ./testfile/gdbinit
	
clean:
	rm *.o *.bin *.asm
