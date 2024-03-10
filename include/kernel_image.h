#ifndef __KERNEL_IMAGE_H__
#define __KERNEL_IMAGE_H__

#include <config.h>

extern "C" int __kernel_image_start_vma, __kernel_image_start_lma,
       __kernel_image_end_vma, __kernel_image_end_lma;

#if defined(CONFIG_MULTIBOOT2) && (CONFIG_ARCH == ARCH_i386 || CONFIG_ARCH == ARCH_x86_64)
	#include <x86/multiboot2/sections.h>
	#include <x86/multiboot2/segments.h>
#endif

#endif
