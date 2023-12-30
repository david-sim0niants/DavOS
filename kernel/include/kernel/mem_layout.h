#ifndef __KERNEL__MEM_LAYOUT_H__
#define __KERNEL__MEM_LAYOUT_H__

#include <stdint.h>
#include <stddef.h>

#include <ldsym.h>


namespace kernel {

struct KernelSection {
	uintptr_t start_vma, start_lma; size_t size;
};

struct KernelMemLayout {
	KernelSection text, bss, rodata, data;
	uintptr_t image_end_lma, image_end_vma;
};

inline KernelMemLayout get_mem_layout()
{
	return {
		.text = {
			.start_vma = (uintptr_t)__ldsym__kernel_text_start_vma,
			.start_lma = (uintptr_t)__ldsym__kernel_text_start_lma,
			.size = (size_t)__ldsym__kernel_text_size,
		},
		.bss = {
			.start_vma = (uintptr_t)__ldsym__kernel_bss_start_vma,
			.start_lma = (uintptr_t)__ldsym__kernel_bss_start_lma,
			.size = (size_t)__ldsym__kernel_bss_size,
		},
		.rodata = {
			.start_vma = (uintptr_t)__ldsym__kernel_rodata_start_vma,
			.start_lma = (uintptr_t)__ldsym__kernel_rodata_start_lma,
			.size = (size_t)__ldsym__kernel_rodata_size,
		},
		.data = {
			.start_vma = (uintptr_t)__ldsym__kernel_data_start_vma,
			.start_lma = (uintptr_t)__ldsym__kernel_data_start_lma,
			.size = (size_t)__ldsym__kernel_data_size,
		},
		.image_end_lma = (uintptr_t)__ldsym__kernel_image_end_lma,
		.image_end_vma = (uintptr_t)__ldsym__kernel_image_end_vma,
	};
}

}

#endif
