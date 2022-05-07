OUTPUT_ARCH( "riscv" )

ENTRY(_start)

BASE_ADDRESS = 0xFFFFFFFFC0200000;

SECTIONS
{
    . = BASE_ADDRESS;
    .text :AT(0x80200000) {
        entry.o(.text)
        kmain.o(.text)
        trap_handler.o(.text)
        *(.text)
    }
    
    . = ALIGN(4096);
    PROVIDE(etext = .);

    .rodata : {
     *(.rodata)
    }
    
    
    
    .data : {
        entry.o(.data)
        *(.data)
    }

    .bss :{
            *(.bss)
    }
    
    PROVIDE(kend = .);
}
