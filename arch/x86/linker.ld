ENTRY(_start)

SECTIONS
{
	. = 1M;

	.i386.text : ALIGN(4K)
	{
		*(.i386.text)
	}

	PROVIDE(_start = ADDR(.i386.text));

	.i386.bss (NOLOAD) : ALIGN(4K)
	{
		QUAD(0)
	}

	.i386.rodata : ALIGN(4K)
	{
		*(.i386.rodata)
	}

	.ldsym :
	{
		PROVIDE(__ldsym__stack_top = .);
		QUAD(__STACK_TOP)
		PROVIDE(__ldsym__kernel_image_end_vma = .);
		QUAD(__KERNEL_IMAGE_END_VMA)
		PROVIDE(__ldsym__kernel_image_end_lma = .);
		QUAD(__KERNEL_IMAGE_END_LMA)

		PROVIDE(__ldsym__kernel_text_start_vma = .);
		QUAD(__KERNEL_TEXT_START_VMA);
		PROVIDE(__ldsym__kernel_text_start_lma = .);
		QUAD(__KERNEL_TEXT_START_LMA);
		PROVIDE(__ldsym__kernel_text_size = .);
		QUAD(__KERNEL_TEXT_SIZE);

		PROVIDE(__ldsym__kernel_bss_start_vma = .);
		QUAD(__KERNEL_BSS_START_VMA);
		PROVIDE(__ldsym__kernel_bss_start_lma = .);
		QUAD(__KERNEL_BSS_START_LMA);
		PROVIDE(__ldsym__kernel_bss_size = .);
		QUAD(__KERNEL_BSS_SIZE);

		PROVIDE(__ldsym__kernel_rodata_start_vma = .);
		QUAD(__KERNEL_RODATA_START_VMA);
		PROVIDE(__ldsym__kernel_rodata_start_lma = .);
		QUAD(__KERNEL_RODATA_START_LMA);
		PROVIDE(__ldsym__kernel_rodata_size = .);
		QUAD(__KERNEL_RODATA_SIZE);

		PROVIDE(__ldsym__kernel_data_start_vma = .);
		QUAD(__KERNEL_DATA_START_VMA);
		PROVIDE(__ldsym__kernel_data_start_lma = .);
		QUAD(__KERNEL_DATA_START_LMA);
		PROVIDE(__ldsym__kernel_data_size = .);
		QUAD(__KERNEL_DATA_SIZE);
	}

	.i386.data : ALIGN(4K)
	{
		*(.i386.data)
	}

	PROVIDE(__STACK_SIZE = DEFINED(__STACK_SIZE) ? __STACK_SIZE : 8K);

	.stack BLOCK(__STACK_SIZE) (NOLOAD) : ALIGN(4K)
	{
		. += __STACK_SIZE;
	}
	PROVIDE(__STACK_TOP = .);

	PROVIDE(__KERNEL_TEXT_START_LMA = .);

	. = __VM_SPLIT;
	. = ALIGN(4K);

	. += __KERNEL_TEXT_START_LMA;
	. = ALIGN(4K);
	PROVIDE(__KERNEL_TEXT_START_VMA = .);

	.text : AT(__KERNEL_TEXT_START_LMA)
	{
		*(.text)
		*(.text.*)
	}

	. = ALIGN(4K);
	PROVIDE(__KERNEL_TEXT_SIZE = . - __KERNEL_TEXT_START_VMA);
	PROVIDE(__KERNEL_BSS_START_VMA = .);
	PROVIDE(__KERNEL_BSS_START_LMA = . - __VM_SPLIT);

	.bss (NOLOAD) : AT(__KERNEL_DATA_START_LMA)
	{
		*(.bss)
	}

	. = ALIGN(4K);
	PROVIDE(__KERNEL_BSS_SIZE = . - __KERNEL_BSS_START_VMA);
	PROVIDE(__KERNEL_RODATA_START_VMA = .);
	PROVIDE(__KERNEL_RODATA_START_LMA = . - __VM_SPLIT);

	.rodata : AT(__KERNEL_RODATA_START_LMA)
	{
		*(.rodata)
		*(.rodata.*)
	}

	. = ALIGN(4K);
	PROVIDE(__KERNEL_RODATA_SIZE = . - __KERNEL_RODATA_START_VMA);
	PROVIDE(__KERNEL_DATA_START_VMA = .);
	PROVIDE(__KERNEL_DATA_START_LMA = . - __VM_SPLIT);

	.data : AT(__KERNEL_DATA_START_LMA)
	{
		*(.data)
		*(.data.*)
	}

	. = ALIGN(4K);
	PROVIDE(__KERNEL_DATA_SIZE = . - __KERNEL_DATA_START_VMA);

	PROVIDE(__KERNEL_IMAGE_END_VMA = .);
	PROVIDE(__KERNEL_IMAGE_END_LMA = . - __VM_SPLIT);

	/DISCARD/ :
	{
		*(.note.gnu.property)
		*(.comment)
		*(.eh_frame)
	}
}
