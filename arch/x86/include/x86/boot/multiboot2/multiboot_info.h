#ifndef _x86_BOOT_MULTIBOOT2__MULTIBOOT_INFO_H__
#define _x86_BOOT_MULTIBOOT2__MULTIBOOT_INFO_H__

#include <stdint.h>

namespace x86 {

struct MultibootInfo {
	const char *boot_cmd_line;

	struct BasicMemInfo {
		uint32_t mem_lower;
		uint32_t mem_upper;
	} basic_mem_info;

	struct MMap {
		enum Type : uint32_t {
			RAM = 1,
			ACPI = 3,
			Reserved= 4,
		};

		uint32_t entry_version;
		uint32_t nr_entries;
		const struct Entry {
			uint64_t base_addr;
			uint64_t length;
			Type type;
			uint32_t reserved;
		} *entries;
	} mmap;
};

/* Reorganizes the multiboot info structure pointed by tags_struct and writes it
 * to the info struct.*/
void read_multiboot_info(const void *tags_struct, MultibootInfo& info);

}

#endif
