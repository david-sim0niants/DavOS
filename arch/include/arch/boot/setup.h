#ifndef __ARCH_BOOT__SETUP_H__
#define __ARCH_BOOT__SETUP_H__

#include <stddef.h>
#include <stdint.h>

namespace arch {

enum class PhysicalMMapType {
	None, RAM, ACPI, Reserved,
};

struct PhysicalMMap {
	struct Entry {
		uintptr_t base_addr;
		size_t length;
		PhysicalMMapType type;
	} *entries;
	size_t nr_entries;
};

/* Opaque type for BootInfo. Arch-specific versions of it contain
 * arch-specific boot information about the system. */
using BootInfo = void;

/* Setup the arch API. */
void setup(BootInfo *boot_info);

BootInfo *get_boot_info();

}

#endif
