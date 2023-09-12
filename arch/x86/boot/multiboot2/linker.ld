ENTRY(_start)

SECTIONS
{
	. = 1M;
	.text : ALIGN(4K)
	{
		KEEP(*(.multiboot_header))
		*(.text)
	}

	.bss (NOLOAD) : ALIGN(4K)
	{
		*(.bss)
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

	.stack :
	{
		KEEP(*(.stack))
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
