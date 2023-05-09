ENTRY(_start)

SECTIONS
{
	. = 1M;

	.multiboot_header : ALIGN(4K)
	{
		KEEP(*(.multiboot_header))
	}

	.text :
	{
		*(.text)
	}

	.data : ALIGN(4K)
	{
		*(.data)
	}

	.rodata : ALIGN(4K)
	{
		*(.rodata)
	}

	PROVIDE(__stack_size = DEFINED(__stack_size) ? __stack_size : 8K);
	PROVIDE(__kernel_start = DEFINED(__kernel_start) ? __kernel_start : 2M);

	. = __kernel_start - __stack_size;
	.stack BLOCK(__stack_size) (NOLOAD) : ALIGN(4K)
	{
		ASSERT(((. + __stack_size) <= __kernel_start),"Error: i386 image is too large and leaves no room for the stack. Increase the __kernel_start.");
		. += __stack_size;
	}

	.kernel_start_placeholder :
	{
		KEEP(*(.kernel_start_placeholder))
	}

	/DISCARD/ :
	{
        	*(.note.*)
        	*(.comment)
		*(.eh_frame)
	}
}
