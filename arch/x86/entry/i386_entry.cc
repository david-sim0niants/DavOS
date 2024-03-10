#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <kernel_image.h>
#include <config.h>
#include <x86/config.h>

#include <x86/ldsym.h>
#include <x86/entry/i386_entry.h>
#include <x86/cpuid.h>
#include <x86/utils/vga/console.h>
#include <x86/utils/vga/ostream.h>
#include <x86/paging.h>
#include <x86/system.h>
#include <x86/boot/setup.h>

#include <kstd/new.h>
#include <kstd/io.h>
#include <kstd/algorithm.h>
#include <kstd/memory.h>
#include <kstd/array.h>

#include "gdt.h"


namespace x86 {

enum class LocalErr {
	None = 0, IdentityMapFail, KernelMapFail,
};

static void print_vendor_info(ArchInfo &arch_info, utils::VGA_OStream& os);
static bool try_x86_cpuid_verbose(ArchInfo& arch_info, utils::VGA_OStream& os);
static kstd::MemoryRange find_free_memory(const BootInfo& boot_info,
		uintptr_t min_addr, unsigned int alignment);

static PageMapErr map_pages__free_mem(BootInfo& boot_info, PageMappingInfo& map_info,
		PageTable *page_table, uintptr_t& min_addr);
static LocalErr identity_map_pages(BootInfo& boot_info, PageTable *page_table, uintptr_t& min_addr);
static LocalErr map_kernel_memory(BootInfo& boot_info, PageTable *page_table, uintptr_t& min_addr);

static void setup_data_segments();

static inline void next_entry(BootInfo *boot_info);


static const char *red_on_black = "\033[5m";
static const char *green_on_black = "\033[3m";
static const char *reset_color = "\033[0m";
static const char *reset_screen = "\033c";


extern "C" void _i386_start(x86::BootInfo *boot_info)
{
	utils::VGA_OStream os;
	os << "\033c";

	ArchInfo arch_info;
	if (!try_x86_cpuid_verbose(arch_info, os)) {
		os << red_on_black << "Can't boot 64bit image.\n" << reset_color;
		halt();
	}

	print_vendor_info(arch_info, os);

	if (kstd::test_flag(arch_info.ext_feature_flags,ExtFeatureFlags::LongMode)) {
		os << green_on_black << "Long mode available.\n" << reset_color;
	} else {
		os 	<< red_on_black
			<< "Long mode unavailable. Can't boot 64bit image.\n"
			<< reset_color;
		halt();
	}

	uintptr_t min_addr = kstd::max((uintptr_t)__ldsym__kernel_image_end_lma,
			(uintptr_t)boot_info + boot_info->size);
	kstd::MemoryRange pt_mem = find_free_memory(*boot_info, min_addr, PageTable::size_shift);
	PageTable *page_table = reinterpret_cast<PageTable *>(pt_mem.beg);
	new (page_table) PageTable();
	min_addr += PageTable::size;

	os << "Current page table pointer: " << page_table << '\n';

	LocalErr e;
	e = identity_map_pages(*boot_info, page_table, min_addr);
	if (e != LocalErr::None) {
		os << red_on_black << "Failed to identity map pages.\n" << reset_color;
		halt();
	}

	e = map_kernel_memory(*boot_info, page_table, min_addr);
	if (e != LocalErr::None) {
		os << red_on_black << "Failed to map kernel memory.\n" << reset_color;
		halt();
	}

	set_curr_pt_ptr((PhysAddr)page_table);
	// TODO: check the cpuid features
	enable_paging();

	os << green_on_black << "Paging enabled!\n";
	next_entry(boot_info);

	os << red_on_black << "i386 ENTRY ERROR: return from next_entry.\n";
	halt();
}


static bool try_x86_cpuid_verbose(ArchInfo& arch_info, utils::VGA_OStream& os)
{
	os << "Checking CPUID.\n";

	if (cpuid(arch_info)) {
		os << green_on_black << "CPUID available.\n" << reset_color;
		return true;
	} else {
		os << red_on_black << "CPUID unavailable.\n" << reset_color;
		return false;
	}
}

static void print_vendor_info(ArchInfo& arch_info, utils::VGA_OStream& os)
{
	os << "Processor vendor: ";

	const size_t vendor_id_len = sizeof(arch_info.vendor_id);
	os.write(arch_info.vendor_id, vendor_id_len);
	os << '\n';
}


static kstd::MemoryRange find_free_memory(const BootInfo& boot_info,
		uintptr_t min_addr, unsigned int alignment)
{
	for (size_t i = 0; boot_info.mmap.nr_entries; ++i){ 
		auto& entry = boot_info.mmap.entries[i];
		if (entry.type != arch::PhysicalMMapType::RAM)
			continue;
		uintptr_t end_addr = entry.base_addr + entry.length;
		if (end_addr <= min_addr)
			continue;

		uintptr_t start_addr = kstd::max(min_addr, entry.base_addr);
		start_addr = kstd::align_ceiled(start_addr, alignment);
		end_addr = kstd::align_floored(end_addr, alignment);
		if (start_addr >= end_addr)
			continue;
		return kstd::MemoryRange((kstd::Byte *)start_addr, (kstd::Byte *)end_addr);
	}
	return {};
}

static PageMapErr map_pages__free_mem(BootInfo& boot_info, PageMappingInfo& map_info,
		PageTable *page_table, uintptr_t& min_addr)
{
	while (true) {
		kstd::MemoryRange free_mem = find_free_memory(boot_info, min_addr,
				PageTableEntry_<1>::controlled_bits);
		if (free_mem.beg >= free_mem.end)
			return PageMapErr::NoFreeMem;
		PageMapErr e = page_table->map_memory(map_info, free_mem);
		min_addr = (uintptr_t)free_mem.beg;
		if (e != PageMapErr::NoFreeMem)
			return e;
		if (e == PageMapErr::None)
			break;
	}
	return PageMapErr::None;
}

static LocalErr identity_map_pages(BootInfo& boot_info, PageTable *page_table, uintptr_t& min_addr)
{
	PageMapErr e = PageMapErr::None;
	PageMappingInfo map_info;

	map_info = {
		.linaddr_beg = 0,
		.phyaddr_beg = 0,
		.phyaddr_end = __ldsym__kernel_image_start_lma,
		.flags  = PageEntryFlags::Global
			| PageEntryFlags::WriteAllowed
			| PageEntryFlags::Supervisor
			| PageEntryFlags::ExecuteDisabled,
	};

	e = map_pages__free_mem(boot_info, map_info, page_table, min_addr);
	if (e != PageMapErr::None)
		return LocalErr::IdentityMapFail;

	auto sections = kernel_image::get_sections();
	for (size_t i = 0 ; i < sections.size(); ++i) {
		const kernel_image::Section& section = sections[i];
		map_info.linaddr_beg = section.lma_start;
		map_info.phyaddr_beg = section.lma_start;
		map_info.phyaddr_end = section.lma_start + section.size;
		map_info.flags = PageEntryFlags::Global | PageEntryFlags::Supervisor
			| kstd::switch_flag(PageEntryFlags::WriteAllowed,
				kstd::test_flag(section.flags, kstd::SectionFlag::Write))
			| kstd::switch_flag(PageEntryFlags::ExecuteDisabled,
				!kstd::test_flag(section.flags, kstd::SectionFlag::Executable));
		e = map_pages__free_mem(boot_info, map_info, page_table, min_addr);
		if (e != PageMapErr::None)
			return LocalErr::IdentityMapFail;
	}

	return LocalErr::None;
}

static LocalErr map_kernel_memory(BootInfo& boot_info, PageTable *page_table, uintptr_t& min_addr)
{
	utils::VGA_OStream os;
	auto sections = kernel_image::get_main_sections();
	for (size_t i = 0 ; i < sections.size(); ++i) {
		const kernel_image::Section& section = sections[i];
		PageMappingInfo map_info {
			.linaddr_beg = page_fit_linear_addr(section.vma_start),
			.phyaddr_beg = section.lma_start,
			.phyaddr_end = section.lma_start + section.size,
			.flags = PageEntryFlags::Global | PageEntryFlags::Supervisor
				| kstd::switch_flag(PageEntryFlags::WriteAllowed,
					kstd::test_flag(section.flags, kstd::SectionFlag::Write))
				| kstd::switch_flag(PageEntryFlags::ExecuteDisabled,
					!kstd::test_flag(section.flags, kstd::SectionFlag::Executable)),
		};
		PageMapErr e = map_pages__free_mem(boot_info, map_info, page_table, min_addr);
		if (e != PageMapErr::None)
			return LocalErr::KernelMapFail;
	}
	return LocalErr::None;
}


static inline void setup_data_segments()
{
	asm volatile (
		"mov %[ds_offset], %%ax 	\n"
		"mov %%ax, %%ds 		\n"
		"mov %%ax, %%es 		\n"
		"mov %%ax, %%fs 		\n"
		"mov %%ax, %%gs 		\n"
		"mov %%ax, %%ss 		\n"
		:: [ds_offset]"i"(sizeof(GDT_Entry) * 2)
	);
}


#if CONFIG_ARCH == ARCH_x86_64
static inline void next_entry(BootInfo *boot_info)
{
	setup_gdt();
	load_gdt();
	setup_data_segments();
	asm volatile (
		"ljmp %[cs_offset], $.Lhere		\n"
	".Lhere: 					\n"
		"mov %[stack_top], %%esp 	\n"
		"mov %[boot_info], %%edi 		\n"
		"jmp *%[x86_64_entry]"
		::
		[boot_info]"r"(boot_info),
		[stack_top]"r"((uint32_t)__ldsym__kernel_stack_top),
		[cs_offset]"i"(sizeof(GDT_Entry)),
		[x86_64_entry]"r"((uint32_t)__ldsym__kernel_x86_64_entry)
	);
}
#else
static inline void next_entry(BootInfo *boot_info)
{
	asm volatile (
		"mov (__ldsym__stack_top), %esp 			\n"
	);
	reinterpret_cast<void (*)(arch::BootInfo *boot_info)>(boot_info);
}
#endif


static void __attribute__((section(".fake_entry"))) fake_entry()
{
	halt();
}

} // x86
