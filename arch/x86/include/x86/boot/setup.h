#ifndef __x86_BOOT__SETUP_H__
#define __x86_BOOT__SETUP_H__

#include <stddef.h>
#include <stdint.h>

#include <arch/boot/setup.h>


namespace x86 {

struct BootInfo {
	size_t size;
	const char *boot_cmd;
	arch::PhysicalMMap mmap;
};

}

#endif
