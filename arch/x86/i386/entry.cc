#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <kernel_image.h>
#include <config.h>
#include <x86/config.h>

#include <x86/ldsym.h>
#include <x86/i386/entry.h>
#include <x86/cpuid.h>
#include <x86/utils/vga/console.h>
#include <x86/utils/vga/ostream.h>
#include <x86/paging.h>
#include <x86/system.h>
#include <x86/boot/setup.h>

#include <kernel/mem_layout.h>

#include <kstd/new.h>
#include <kstd/io.h>
#include <kstd/algorithm.h>
#include <kstd/memory.h>

#include "gdt.h"

namespace x86 {

enum class LocalErr {
	None = 0, IdentityMapFail, KernelMapFail, PreKernelIdentityMapFail,
};

static void print_vendor_info(ArchInfo &arch_info, utils::VGA_OStream& os);
static bool try_x86_cpuid_verbose(ArchInfo& arch_info, utils::VGA_OStream& os);
static kstd::MemoryRange find_free_memory(const BootInfo& boot_info,
		uintptr_t min_addr, unsigned int alignment);

static PageMapErr map_pages__free_mem(BootInfo& boot_info, PageMappingInfo& map_info,
		PageTable *page_table, uintptr_t& min_addr);
static LocalErr identity_map_pages(BootInfo& boot_info, PageTable *page_table, uintptr_t& min_addr);
static LocalErr map_identity_pages_preceding_kernel(kstd::Byte *&map_location,
		PageTable *page_table);

static LocalErr map_kernel_memory(const kernel::KernelMemLayout& mem_layout,
		kstd::Byte *&map_location, PageTable *page_table);

static void setup_data_segments();
static void far_jmp_to_main();


static const char *red_on_black = "\033[5m";
static const char *green_on_black = "\033[3m";
static const char *reset_color = "\033[0m";
static const char *reset_screen = "\033c";


extern "C" void _x86_i386_start(x86::BootInfo *boot_info)
{
	utils::VGA_OStream os;

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
	os << page_table << '\n';

	LocalErr e;
	e = identity_map_pages(*boot_info, page_table, min_addr);
	if (e != LocalErr::None) {
		os << red_on_black << "Failed to identity map pages.\n" << reset_color;
		halt();
	}

	// auto e = map_identity_pages_preceding_kernel(pt_mem.ptr, );
	// if (e != LocalErr::None) {
	// 	os 	<< red_on_black
	// 		<< "Failed to identity map pre kernel start memory."
	// 		<< reset_color;
	// 	halt();
	// }

	kernel::KernelMemLayout mem_layout = kernel::get_mem_layout();

	// e = map_kernel_memory(kernel::get_mem_layout(), map_location, page_table);
	// if (e != LocalErr::None) {
	// 	os << red_on_black << "Failed to map kernel memory.\n" << reset_color;
	// 	halt();
	// }

	set_curr_pt_ptr((PhysAddr)page_table);
	// TODO: check the cpuid features
	enable_paging();

	halt();

	load_gdt();
	setup_data_segments();
	far_jmp_to_main();
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
		if (((start_addr >> alignment) << alignment) != start_addr)
			start_addr = ((start_addr >> alignment) + 1) << alignment;
		end_addr = (end_addr >> alignment) << alignment;
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
		PageMapErr e = page_table->map_memory(map_info, free_mem);
		if (e == PageMapErr::None)
			break;
		if (e != PageMapErr::NoFreeMem)
			return e;
		if (free_mem.beg >= free_mem.end)
			return PageMapErr::NoFreeMem;
		min_addr = (uintptr_t)free_mem.end;
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
		.phyaddr_end = (PhysAddr)__ldsym__kernel_image_start_lma,
		.flags  = PageEntryFlags::Global
			| PageEntryFlags::Supervisor
			| PageEntryFlags::WriteAllowed
			| PageEntryFlags::ExecuteDisabled,
	};

	e = map_pages__free_mem(boot_info, map_info, page_table, min_addr);
	if (e != PageMapErr::None)
		return LocalErr::IdentityMapFail;

	auto sections = kernel_image::get_sections();
	for (size_t i = 0 ; i < sections.size(); ++i) {
		const kstd::Section& section = sections[i];
		map_info.linaddr_beg = section.lma_start;
		map_info.phyaddr_beg = section.lma_start;
		map_info.phyaddr_end = section.lma_start + section.size;
		map_info.flags = PageEntryFlags::Global | PageEntryFlags::Supervisor;
		map_info.flags |= kstd::switch_flag(PageEntryFlags::ExecuteDisabled,
			(section.flags & kstd::SectionFlag::Executable) != kstd::SectionFlag::Executable);
		map_info.flags |= kstd::switch_flag(PageEntryFlags::WriteAllowed,
			(section.flags & kstd::SectionFlag::Write) == kstd::SectionFlag::Write);
		e = map_pages__free_mem(boot_info, map_info, page_table, min_addr);
		if (e != PageMapErr::None)
			return LocalErr::IdentityMapFail;
	}

	return LocalErr::None;
}

static LocalErr map_identity_pages_preceding_kernel(kstd::Byte *&map_location,
		PageTable *page_table)
{
	PageMappingInfo map_info = {
		.linaddr_beg = 0,
		.phyaddr_beg = 0,
		.phyaddr_end = (PhysAddr)__ldsym__kernel_text_start_lma,
		.flags  = PageEntryFlags::Global
			| PageEntryFlags::Supervisor
			| PageEntryFlags::WriteAllowed,
	};

	kstd::MemoryRange free_mem = {map_location, map_location + 0x10000000};

	auto e = page_table->map_memory(map_info, free_mem);
	if (e != PageMapErr::None)
		return LocalErr::PreKernelIdentityMapFail;

	return LocalErr::None;
}

static LocalErr map_kernel_memory(const kernel::KernelMemLayout &mem_layout,
	kstd::Byte *&map_location, PageTable *page_table)
{
	PageMapErr e;

	PageMappingInfo map_info = {
		.linaddr_beg = (LineAddr)mem_layout.text.start_vma,
		.phyaddr_beg = (PhysAddr)mem_layout.text.start_lma,
		.phyaddr_end = (PhysAddr)(mem_layout.text.start_lma)
				+ mem_layout.text.size,
		.flags  = PageEntryFlags::Global
			| PageEntryFlags::Supervisor
	};

	kstd::MemoryRange free_mem = {map_location, map_location + 0x10000000};

	e = page_table->map_memory(map_info, free_mem);
	if (e != PageMapErr::None)
		return LocalErr::KernelMapFail;

	map_info = {
		.linaddr_beg = (LineAddr)mem_layout.bss.start_vma,
		.phyaddr_beg = (PhysAddr)mem_layout.bss.start_lma,
		.phyaddr_end = (PhysAddr)(mem_layout.bss.start_lma)
				+ mem_layout.bss.size,
		.flags  = PageEntryFlags::Global
			| PageEntryFlags::Supervisor
			| PageEntryFlags::WriteAllowed
			| PageEntryFlags::ExecuteDisabled,
	};

	e = page_table->map_memory(map_info, free_mem);
	if (e != PageMapErr::None)
		return LocalErr::KernelMapFail;

	map_info = {
		.linaddr_beg = (LineAddr)mem_layout.rodata.start_vma,
		.phyaddr_beg = (PhysAddr)mem_layout.rodata.start_lma,
		.phyaddr_end = (PhysAddr)(mem_layout.rodata.start_lma)
				+ mem_layout.rodata.size,
		.flags  = PageEntryFlags::Global
			| PageEntryFlags::Supervisor
			| PageEntryFlags::ExecuteDisabled,
	};

	e = page_table->map_memory(map_info, free_mem);
	if (e != PageMapErr::None)
		return LocalErr::KernelMapFail;

	map_info = {
		.linaddr_beg = (LineAddr)mem_layout.data.start_vma,
		.phyaddr_beg = (PhysAddr)mem_layout.data.start_lma,
		.phyaddr_end = (PhysAddr)(mem_layout.data.start_lma)
				+ mem_layout.data.size,
		.flags  = PageEntryFlags::Global
			| PageEntryFlags::Supervisor
			| PageEntryFlags::WriteAllowed
			| PageEntryFlags::ExecuteDisabled,
	};

	e = page_table->map_memory(map_info, free_mem);
	if (e != PageMapErr::None)
		return LocalErr::KernelMapFail;

	return LocalErr::None;
}


static __FORCE_INLINE void setup_data_segments()
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


static __FORCE_INLINE void far_jmp_to_main()
{
	uint32_t kernel_main = (uint32_t)__ldsym__kernel_main;

	asm volatile (
		"ljmp %[cs_offset], $.Lhere 	\n"
		".Lhere: 			\n"
		"jmp *%[main] 			\n"
		::
		[cs_offset]"i"(sizeof(GDT_Entry)),
		[main]"r"(kernel_main)
		:
		"memory"
	);
	__builtin_unreachable();
}


static void __attribute__((section(".fake_main_text"))) fake_main()
{
	halt();
}

} // x86
