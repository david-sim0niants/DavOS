#include <x86/boot/multiboot2/multiboot_info.h>
#include <x86/boot/setup.h>
#include <x86/i386/entry.h>

#include <ldsym.h>

#include <kstd/new.h>
#include <kstd/algorithm.h>
#include <kstd/overflow.h>

#include <string.h>

namespace x86 {

static arch::PhysicalMMapType mb2_to_boot_info_mmap_type(MultibootInfo::MMap::Type type);
static uintptr_t find_free_memory(MultibootInfo::MMap& mmap,
		uintptr_t min_allowed_addr, size_t required_size);

/* Entry called by the multiboot2 compliant bootstrap code. */
extern "C" int _mb2_start(void *mb_info_tags_struct)
{
	// read multiboot2 info to convert it into BootInfo abstract structure
	MultibootInfo mb_info;
	read_multiboot_info(mb_info_tags_struct, mb_info);

	const size_t boot_cmd_size = strlen(mb_info.boot_cmd_line) + 1;

	// +1 is for reserving memory used by BootInfo itself
	const size_t mmap_entries_mem_size
		= mb_info.mmap.nr_entries * sizeof(arch::PhysicalMMap::Entry);

	// calculate total size required by the BootInfo,
	// including the structure size and auxiliary memory
	size_t boot_info_mem_size = sizeof(BootInfo) + mmap_entries_mem_size + boot_cmd_size;

	// we don't want to overwrite the kernel image memory,
	// so we allocate free memory coming after it
	const uintptr_t min_allowed_addr = __ldsym__kernel_image_end_lma;
	// find free memory for boot info
	uintptr_t boot_info_addr = find_free_memory(
			mb_info.mmap, min_allowed_addr, boot_info_mem_size);
	if (!boot_info_addr)
		return 1;

	// initialize the boot info
	BootInfo *boot_info = reinterpret_cast<BootInfo *>(boot_info_addr);
	new (boot_info) BootInfo();

	// assign total boot info memory size
	boot_info->size = boot_info_mem_size;

	// copy boot command
	char *boot_cmd = reinterpret_cast<char *>(boot_info + 1);
	boot_info->boot_cmd = boot_cmd;
	strcpy(boot_cmd, mb_info.boot_cmd_line);

	// copy mmap entries
	auto *entries = reinterpret_cast<arch::PhysicalMMap::Entry *>(
			boot_cmd + boot_cmd_size + 1);
	boot_info->mmap.nr_entries = static_cast<size_t>(mb_info.mmap.nr_entries);
	boot_info->mmap.entries = entries;
	for (uint32_t i = 0; i < boot_info->mmap.nr_entries; ++i) {
		entries[i].base_addr = (uintptr_t)mb_info.mmap.entries[i].base_addr;
		entries[i].length = (size_t)mb_info.mmap.entries[i].length;
		entries[i].type = mb2_to_boot_info_mmap_type(mb_info.mmap.entries[i].type);
	}

	// call "bootloader-independent" entry passing boot_info
	_x86_i386_start(boot_info);

	// returning from _x86_i386_start is a clear bug
	__builtin_unreachable();

	return 0;
}

static uintptr_t find_free_memory(MultibootInfo::MMap& mmap,
		uintptr_t min_allowed_addr, size_t required_size)
{
	for (uint32_t i = 0; i < mmap.nr_entries; ++i) {
		auto& entry = mmap.entries[i];
		// exclude non-RAM memory
		if (entry.type != MultibootInfo::MMap::RAM)
			continue;
		const uintptr_t end_addr = entry.base_addr + entry.length;
		// exclude memory chunk preceding the min allowed address
		if (min_allowed_addr > end_addr)
			continue;
		// min begin address
		const uintptr_t beg_addr =
			kstd::max(min_allowed_addr, (uintptr_t)entry.base_addr);
		// if remaining size in the memory chunk is enough, return beg_addr
		// end_addr - beg_addr guaranteed to not overflow
		if (end_addr - beg_addr >= required_size)
			return beg_addr;
	}
	return 0;
}

static arch::PhysicalMMapType mb2_to_boot_info_mmap_type(MultibootInfo::MMap::Type type)
{
	switch (type) {
	case MultibootInfo::MMap::RAM:
		return arch::PhysicalMMapType::RAM;
	case MultibootInfo::MMap::ACPI:
		return arch::PhysicalMMapType::ACPI;
	case MultibootInfo::MMap::Reserved:
		return arch::PhysicalMMapType::Reserved;
	default:
		return arch::PhysicalMMapType::None;
	}
}
 
}
