CROSS_COMPILE = riscv64-unknown-elf-


CFLAGS = -nostdlib -fno-builtin -g -Wall
CFLAGS += -mcmodel=medany -mabi=lp64f -march=rv64imafc
CFLAGS += -ggdb

#CFLAGS += -ffreestanding -fno-common -I.
#CFLAGS += -Bstatic
#CFLAGS += -nostartfiles
#k210手册中显示RV64IMAFDC

QEMU = qemu-system-riscv64
#QFLAGS = -nographic -smp 1 -M virt -m 6M -bios none

CC = ${CROSS_COMPILE}gcc
LD = ${CROSS_COMPILE}ld
OBJCOPY = ${CROSS_COMPILE}objcopy
OBJDUMP = ${CROSS_COMPILE}objdump

#riscv64-unknown-elf-objdump -D -b binary -m riscv  '/media/psf/Home/Downloads/hello_world.bin' > asm.asm
#riscv64-unknown-elf-readelf -a -W
#riscv64-unknown-elf-objdump -D 

GDB = gdb-multiarch

#gdb-multiarch -q bootblock.o
#target remote tcp:localhost:1234
#set disassemble-next-line on
#set architecture riscv

#display/z $a4
#display/z $a5
#display/z $sp
#display/z $t0
