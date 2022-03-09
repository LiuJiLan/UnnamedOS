OUTPUT_ARCH( "riscv" )

ENTRY(start)

BASE_ADDRESS = 0x80000000;

SECTIONS
{
    . = BASE_ADDRESS;
    .text : AT(0x80000000) {
        sbi_asm.o(.text)
        sbi_trap_handler.o(.text)
        sbi_main.o(.text)
        *(.text)
    }

    .rodata : {
     *(.rodata)
    }
    
    .data : {
        *(.data)
    }

    .bss :{
            *(.bss)
    }
    
    PROVIDE(sbi_end = .);
}

