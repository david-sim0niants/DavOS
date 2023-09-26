#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <config.h>
#include <x86/config.h>
#include <ldsym.h>

#include <x86/cpuid.h>
#include <x86/vga_text.h>
#include <x86/paging.h>
#include <x86/system.h>

#include <kstd/new.h>

#include "gdt.h"

namespace x86 {

enum LocalErr {
	None = 0, KernelMapFail, PreKernelIdentityMapFail,
};

static void print_vendor_info(VGAText &vga_text, ArchInfo &arch_info);
static bool try_x86_cpuid_verbose(ArchInfo &arch_info, VGAText &vga_text);

struct KernelSection {
	void *start_vma, *start_lma; size_t size;
};

struct KernelMemLayout {
	KernelSection text, bss, rodata, data;
};

static LocalErr map_identity_pages_preceding_kernel(kstd::Byte *&map_location,
		PageTable *page_table);

static LocalErr map_kernel_memory(const KernelMemLayout &mem_layout,
		kstd::Byte *&map_location, PageTable *page_table);

constexpr auto red_on_black =
	VGAText::make_color(VGATextColors::Red, VGATextColors::Black);
constexpr auto green_on_black =
	VGAText::make_color(VGATextColors::Green, VGATextColors::Black);
constexpr auto white_on_black =
	VGAText::make_color(VGATextColors::White, VGATextColors::Black);

static void setup_data_segments();
static void far_jmp_to_main();


extern "C" void _x86_i386_start()
{
	VGAText::clear_screen();

	VGAText vga_text;
	ArchInfo arch_info;

	if (!try_x86_cpuid_verbose(arch_info, vga_text)) {
		vga_text.set_color(red_on_black);
		vga_text.puts("Can't boot 64bit image.\r\n");
		halt();
	}

	print_vendor_info(vga_text, arch_info);

	if (kstd::test_flag(arch_info.ext_feature_flags,ExtFeatureFlags::LongMode)) {
		vga_text.set_color(green_on_black);
		vga_text.puts("Long mode available.\r\n");
	} else {
		vga_text.set_color(red_on_black);
		vga_text.puts(
			"Long mode unavailable. Can't boot 64bit image.\r\n");
		halt();
	}


	KernelMemLayout mem_layout = {
		.text = {
			.start_vma = (void *)__ldsym__kernel_text_start_vma,
			.start_lma = (void *)__ldsym__kernel_text_start_lma,
			.size = (size_t)__ldsym__kernel_text_size,
		},
		.bss = {
			.start_vma = (void *)__ldsym__kernel_bss_start_vma,
			.start_lma = (void *)__ldsym__kernel_bss_start_lma,
			.size = (size_t)__ldsym__kernel_bss_size,
		},
		.rodata = {
			.start_vma = (void *)__ldsym__kernel_rodata_start_vma,
			.start_lma = (void *)__ldsym__kernel_rodata_start_lma,
			.size = (size_t)__ldsym__kernel_rodata_size,
		},
		.data = {
			.start_vma = (void *)__ldsym__kernel_data_start_vma,
			.start_lma = (void *)__ldsym__kernel_data_start_lma,
			.size = (size_t)__ldsym__kernel_data_size,
		},
	};

	auto map_location_val = (unsigned long)__ldsym__kernel_image_end_lma;
	auto map_location_pn = map_location_val / PageTable::size;
	if (map_location_pn * PageTable::size != map_location_val)
		map_location_val = (map_location_pn + 1) * PageTable::size;

	vga_text.set_color(green_on_black);

	kstd::Byte *map_location = reinterpret_cast<kstd::Byte *>(map_location_val);

	PageTable *page_table = new (map_location) PageTable;

	map_location += PageTable::size;

	auto e = map_identity_pages_preceding_kernel(map_location, page_table);
	if (e != LocalErr::None) {
		vga_text.set_color(red_on_black);
		vga_text.puts(
			"Failed to identity map pre kernel start memory.");
		halt();
	}

	e = map_kernel_memory(mem_layout, map_location, page_table);
	if (e != LocalErr::None) {
		vga_text.set_color(red_on_black);
		vga_text.puts("Failed to map kernel memory.");
		halt();
	}

	set_curr_pt_ptr((PhysAddr)page_table);
	// TODO: check the cpuid features
	enable_paging();

	load_gdt();
	setup_data_segments();
	far_jmp_to_main();
}


static bool try_x86_cpuid_verbose(ArchInfo &arch_info, VGAText &vga_text)
{
	vga_text.puts("Checking CPUID.\r\n");

	if (cpuid(arch_info)) {
		vga_text.set_color(green_on_black);
		vga_text.puts("CPUID available.\r\n");
		return true;
	} else {
		vga_text.set_color(red_on_black);
		vga_text.puts("CPUID unavailable.\r\n");
		return false;
	}
}

static void print_vendor_info(VGAText &vga_text, ArchInfo &arch_info)
{
	vga_text.set_color(white_on_black);

	vga_text.puts("Processor vendor: ");

	const size_t vendor_id_len = sizeof(arch_info.vendor_id);
	vga_text.write_buffer(arch_info.vendor_id, vendor_id_len);
	vga_text.puts("\r\n");
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

static LocalErr map_kernel_memory(const KernelMemLayout &mem_layout,
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
