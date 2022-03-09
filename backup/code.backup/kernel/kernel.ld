OUTPUT_ARCH( "riscv" )

ENTRY(_start)

BASE_ADDRESS = 0xFFFFFFFFC0200000;

SECTIONS
{
    . = BASE_ADDRESS;
    .text :AT(0x80200000) {
        entry.o(.text)
        main.o(.text)
        *(.text)
    }

    .rodata : {
     *(.rodata)
    }
    
    PROVIDE(data = .);
    
    .data : {
        . = ALIGN(4096);
        *(.data)
    }

    .bss :{
            *(.bss)
    }
    
        PROVIDE(end = .);
}
