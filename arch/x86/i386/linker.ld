ENTRY(_start)

SECTIONS
{
	. = 1M;

	.text : ALIGN(4K)
	{
		KEEP(*(.multiboot_header))
		*(.text)
		*(.text.*)
	}

	.bss (NOLOAD) : ALIGN(4K)
	{
		*(.bss)
	}

	.rodata : ALIGN(4K)
	{
		*(.rodata)
		*(.rodata.*)
	}

	.ldsym :
	{
		PROVIDE(__ldsym__stack_top = .);
		QUAD(__STACK_TOP)
		PROVIDE(__ldsym__kernel_image_end_vma = .);
		QUAD(0)
		PROVIDE(__ldsym__kernel_image_end_lma = .);
		QUAD(0)

		PROVIDE(__ldsym__kernel_text_start_vma = .);
		QUAD(__KERNEL_START_PLACEHOLDER);
		PROVIDE(__ldsym__kernel_text_start_lma = .);
		QUAD(__KERNEL_START_PLACEHOLDER);
		PROVIDE(__ldsym__kernel_text_size = .);
		QUAD(__KERNEL_PLACEHOLDER_SIZE);

		PROVIDE(__ldsym__kernel_bss_start_vma = .);
		QUAD(0);
		PROVIDE(__ldsym__kernel_bss_start_lma = .);
		QUAD(0);
		PROVIDE(__ldsym__kernel_bss_size = .);
		QUAD(0);

		PROVIDE(__ldsym__kernel_rodata_start_vma = .);
		QUAD(0);
		PROVIDE(__ldsym__kernel_rodata_start_lma = .);
		QUAD(0);
		PROVIDE(__ldsym__kernel_rodata_size = .);
		QUAD(0);

		PROVIDE(__ldsym__kernel_data_start_vma = .);
		QUAD(0);
		PROVIDE(__ldsym__kernel_data_start_lma = .);
		QUAD(0);
		PROVIDE(__ldsym__kernel_data_size = .);
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
