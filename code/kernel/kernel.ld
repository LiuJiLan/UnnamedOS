OUTPUT_ARCH( "riscv" )

ENTRY(_start)

BASE_ADDRESS = 0xFFFFFFFFC0200000;

SECTIONS
{
    . = BASE_ADDRESS
    .text : {
        entry.o(.text)
        *(.text)
    }

    .rodata : {
     *(.rodata)
    }

    .data : {
        . = ALIGN(4096);
        *(.data)
    }

    .bss :{
            *(.bss)
    }
}
