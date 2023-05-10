ENTRY(_start)

SECTIONS
{
	. = 1M;

	.multiboot_header BLOCK (4K) : ALIGN(4K)
	{
		KEEP(*(.multiboot_header.i386))
	}

	. = 1M + 4K;

	.text.i386 : ALIGN(4K)
	{
		*(.text.i386)
	}

	PROVIDE(_start = ADDR(.text.i386));

	.data.i386 : ALIGN(4K)
	{
		*(.data.i386)
	}

	.rodata.i386 : ALIGN(4K)
	{
		*(.rodata.i386)
	}

	PROVIDE(__stack_size = DEFINED(__stack_size) ? __stack_size : 8K);
	PROVIDE(__kernel_start = DEFINED(__kernel_start) ? __kernel_start : 2M);

	. = __kernel_start - __stack_size;
	.stack BLOCK(__stack_size) (NOLOAD) : ALIGN(4K)
	{
		ASSERT(((. + __stack_size) <= __kernel_start),"Error: i386 image is too large and leaves no room for the stack. Increase the __kernel_start.");
		. += __stack_size;
	}

	.text : ALIGN(4K)
	{
		*(.text)
	}

	.data : ALIGN(4K)
	{
		*(.data)
	}

	.bss (NOLOAD) : ALIGN(4K)
	{
		*(.bss)
	}

	/DISCARD/ :
	{
		*(.note.gnu.property)
		*(.comment)
		*(.eh_frame)
	}
}
