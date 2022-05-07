OUTPUT_ARCH(riscv)
ENTRY(_start)

SECTIONS {
    . = 0x1000;
    .text : { 
        *(.text.entry)
        *(.text .text.*)
    }

    .bss : { 
        *(.bss .bss.*)
        *(.sbss .sbss.*)
    }

    .data : { 
        *(.data .rodata) 
        *(.sdata .sdata.*)
    }
    
    /DISCARD/ : { *(.eh_*) }
}
