OUTPUT_ARCH( "riscv" )

ENTRY(start)


SECTIONS
{
	.text 0x01000: {
        bootasm.o(.text)
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
