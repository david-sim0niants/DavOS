ENTRY(_start)

SECTIONS
{
	. = 1M;

	.multiboot_header BLOCK(4K) : ALIGN(4K)
	{
		KEEP(*(.multiboot_header))
	}

	. = 1M + 4K;

	.text : ALIGN(4K)
	{
		*(.text)
	}

	.rodata : ALIGN(4K)
	{
		*(.rodata)
	}

	.ldconfig :
	{
		KEEP(*(.ldconfig))
	}

	.data : ALIGN(4K)
	{
		*(.data)
	}

	PROVIDE(__STACK_SIZE = DEFINED(__STACK_SIZE) ? __STACK_SIZE : 8K);

	.stack BLOCK(__STACK_SIZE) (NOLOAD) : ALIGN(4K)
	{
		. += __STACK_SIZE;
	}

	PROVIDE(__STACK_TOP = .);

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
