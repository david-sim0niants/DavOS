	DEFINE_LDSYM(stack_top, 		__STACK_TOP)
	DEFINE_LDSYM(kernel_image_end_vma, 	0)
	DEFINE_LDSYM(kernel_image_end_lma, 	0)

	DEFINE_LDSYM(kernel_text_start_vma, 	__KERNEL_START_PLACEHOLDER)
	DEFINE_LDSYM(kernel_text_start_lma, 	__KERNEL_START_PLACEHOLDER)
	DEFINE_LDSYM(kernel_text_size, 		__KERNEL_PLACEHOLDER_SIZE)

	DEFINE_LDSYM(kernel_bss_start_vma, 	0)
	DEFINE_LDSYM(kernel_bss_start_lma, 	0)
	DEFINE_LDSYM(kernel_bss_size, 		0)

	DEFINE_LDSYM(kernel_rodata_start_vma, 	0)
	DEFINE_LDSYM(kernel_rodata_start_lma, 	0)
	DEFINE_LDSYM(kernel_rodata_size, 	0)

	DEFINE_LDSYM(kernel_data_start_vma, 	0)
	DEFINE_LDSYM(kernel_data_start_lma, 	0)
	DEFINE_LDSYM(kernel_data_size, 		0)
