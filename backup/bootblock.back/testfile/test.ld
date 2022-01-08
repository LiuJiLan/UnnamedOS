
OUTPUT_ARCH( "riscv" )

ENTRY(start)


MEMORY
{
	ram   : ORIGIN = 0x80000000, LENGTH = 8M
}


SECTIONS
{
	
	.text 0x80200000: {
        bootasm.o(.text)
        *(.text)
	} >ram

	.rodata : {
     *(.rodata)
	} >ram

	.data : {
		. = ALIGN(4096);
        *(.data)
	} >ram

	.bss :{
		    *(.bss)
	} >ram
}
