CROSS_COMPILE = riscv64-unknown-elf-

CFLAGS = -nostdlib -fno-builtin -g -ggdb -Wall
CFLAGS += -mcmodel=medany -mabi=lp64f -march=rv64imafc
#k210手册中显示RV64IMAFDC

QEMU = qemu-system-riscv64
QFLAGS = -smp 1 -M virt -m 6M -bios none
QFLAGS += -nographic

CC = ${CROSS_COMPILE}gcc
LD = ${CROSS_COMPILE}ld
OBJCOPY = ${CROSS_COMPILE}objcopy
OBJDUMP = ${CROSS_COMPILE}objdump

PYTHON = python3

#GDB = gdb-multiarch
GDB = ${CROSS_COMPILE}gdb
