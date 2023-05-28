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
		*(.text.*)
	}

	.rodata : ALIGN(4K)
	{
		*(.rodata)
	}

	.ldconfig :
	{
		PROVIDE(__ldconfig__STACK_TOP = .);
		QUAD(__STACK_TOP)
		PROVIDE(__ldconfig__KERNEL_IMAGE_END_VMA = .);
		QUAD(0)
		PROVIDE(__ldconfig__KERNEL_IMAGE_END_LMA = .);
		QUAD(0)

		PROVIDE(__ldconfig__KERNEL_TEXT_START_VMA = .);
		QUAD(__KERNEL_START_PLACEHOLDER);
		PROVIDE(__ldconfig__KERNEL_TEXT_START_LMA = .);
		QUAD(__KERNEL_START_PLACEHOLDER);
		PROVIDE(__ldconfig__KERNEL_TEXT_SIZE = .);
		QUAD(__KERNEL_PLACEHOLDER_SIZE);

		PROVIDE(__ldconfig__KERNEL_BSS_START_VMA = .);
		QUAD(0);
		PROVIDE(__ldconfig__KERNEL_BSS_START_LMA = .);
		QUAD(0);
		PROVIDE(__ldconfig__KERNEL_BSS_SIZE = .);
		QUAD(0);

		PROVIDE(__ldconfig__KERNEL_RODATA_START_VMA = .);
		QUAD(0);
		PROVIDE(__ldconfig__KERNEL_RODATA_START_LMA = .);
		QUAD(0);
		PROVIDE(__ldconfig__KERNEL_RODATA_SIZE = .);
		QUAD(0);

		PROVIDE(__ldconfig__KERNEL_DATA_START_VMA = .);
		QUAD(0);
		PROVIDE(__ldconfig__KERNEL_DATA_START_LMA = .);
		QUAD(0);
		PROVIDE(__ldconfig__KERNEL_DATA_SIZE = .);
		QUAD(0);
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
	PROVIDE(__KERNEL_START_PLACEHOLDER = .);

	.kernel_start_placeholder :
	{
		KEEP(*(.kernel_start_placeholder))
	}

	PROVIDE(__KERNEL_PLACEHOLDER_SIZE = . - __KERNEL_START_PLACEHOLDER);

	/DISCARD/ :
	{
        	*(.note.*)
        	*(.comment)
		*(.eh_frame)
	}
}
