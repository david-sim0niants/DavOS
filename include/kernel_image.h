#ifndef __KERNEL_IMAGE_H__
#define __KERNEL_IMAGE_H__

#include <config.h>
#include <kstd/section.h>

#if defined(CONFIG_MULTIBOOT2) && (CONFIG_ARCH == ARCH_i386 || CONFIG_ARCH == ARCH_x86_64)
	#include <x86/multiboot2/sections.h>
#endif

#endif
