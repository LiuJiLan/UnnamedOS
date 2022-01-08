//暂时写的很简陋

OUTPUT_ARCH( "riscv" )

ENTRY(start)


SECTIONS
{
	.text 0x80200000: {
        bootasm.o(.text)    // 要把bootasm放在前面
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
