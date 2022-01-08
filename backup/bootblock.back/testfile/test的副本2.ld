
OUTPUT_ARCH( "riscv" )

ENTRY(start)


MEMORY
{
	rom   : ORIGIN = 0x00001000, LENGTH = 128M
}


SECTIONS
{
	
	.text 0x00001000: {
        bootasm.o(.text)
        *(.text)
	} >rom

	.rodata : {
     *(.rodata)
	} >rom

	.data : {
		. = ALIGN(4096);
        *(.data)
	} >rom

	.bss :{
		    *(.bss)
	} >rom
}
