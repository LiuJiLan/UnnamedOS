CROSS_COMPILE = riscv64-unknown-elf-

CFLAGS = -nostdlib -fno-builtin -g -ggdb -Wall
CFLAGS += -mcmodel=medany -mabi=lp64f -march=rv64imafc
#k210手册中显示RV64IMAFDC

QEMU = qemu-system-riscv64
QFLAGS = -smp 3 -M virt -bios none
QFLAGS += -nographic
#QFLAGS += -serial pipe:/tmp/guest

#mkfifo /tmp/guest.in /tmp/guest.out
#cat /tmp/guest.out
#printf "sth" > /tmp/guest.in

CC = ${CROSS_COMPILE}gcc
LD = ${CROSS_COMPILE}ld
OBJCOPY = ${CROSS_COMPILE}objcopy
OBJDUMP = ${CROSS_COMPILE}objdump
READELF = ${CROSS_COMPILE}readelf

PYTHON = python3

#GDB = gdb-multiarch
GDB = ${CROSS_COMPILE}gdb
