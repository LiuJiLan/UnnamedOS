include ../common/common.mk

# 以下部分改写自 CMakeLists
ARCH_DIR = lib/arch/riscv
ASM_DIR = asm
BIN_DIR = bin

LIB_DIR = ./lib

CMAKE_C_FLAGS = -march=rv64imac -mabi=lp64 -mcmodel=medany
CMAKE_C_FLAGS += -fno-builtin -nostdinc -fno-stack-protector -ggdb -Wall
CMAKE_ASM_FLAGS = $(CMAKE_C_FLAGS)
CMAKE_C_LINK_FLAGS = -nostdlib -T ./${ARCH_DIR}/user.ld

LIBS_C = $(shell find $(LIB_DIR) -maxdepth 1 -name "*.c")
LIBS_S = $(shell find $(LIB_DIR) -name "*.S")

ENTRY = 0x1000
SRCS = $(shell find ./src -name "*.c")
#EXECUTABLE_OUTPUT_PATH = ../out/build/
OUTPUT_PATH = ../out/build

SYSCALL_PATH = ./$(ARCH_DIR)/syscall_ids.h.in

syscall_ids.h:
	@sed -n -e 's/__NR_/SYS_/p' $(SYSCALL_PATH) > $(ARCH_DIR)/syscall_ids.h
	
INCLUDE_PATH = ./include
CMAKE_C_FLAGS += -I$(INCLUDE_PATH)
CMAKE_C_FLAGS += -I$(LIB_DIR)
CMAKE_C_FLAGS += -I$(ARCH_DIR)

LIBS_O = $(LIBS_C:.c=.o)
LIBS_O += $(LIBS_S:.S=.o)

SRCS_C = $(SRCS:.c=.o)
SRCS_ELF = $(SRCS:.c=.elf)

%.o : %.c
	@${CC} ${CMAKE_C_FLAGS} -c -o $@ $<
	
%.o : %.S
	@${CC} ${CMAKE_C_FLAGS} -c -o $@ $<

%.elf : %.o $(LIBS_O)
	@${CC} ${CMAKE_C_FLAGS} $(CMAKE_C_LINK_FLAGS) -o $@ $^

.DEFAULT_GOAL := all
all: syscall_ids.h $(SRCS_ELF)

#TEST_DIR += $(OUTPUT_PATH)/src
#TEST_DIR += $(OUTPUT_PATH)/$(ARCH_DIR)

#test_dir:
#	$(foreach DIR,$(TEST_DIR),$(shell if [ ! -e $(DIR) ];then mkdir -p $(DIR); fi))
	
clean:
	@rm -f $(ARCH_DIR)/syscall_ids.h $(SRCS_ELF) $(LIBS_O) $(SRCS_C)
	@rm -f ./src/init.elf ./src/init.bin

init: syscall_ids.h ./src/init.elf
	@${OBJCOPY} -O binary ./src/init.elf ./src/init.bin
