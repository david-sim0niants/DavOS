#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <config.h>
#include <x86/config.h>
#include <ldconfig.h>

#include <x86/cpuid.h>
#include <x86/vga_text.h>
#include <x86/paging.h>

#include <kstd/new.h>

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

static LocalErr map_identity_pages_preceding_kernel(uintptr_t &map_location,
		PageTable *page_table);

static LocalErr map_kernel_memory(const KernelMemLayout &mem_layout,
		uintptr_t &map_location, PageTable *page_table);

constexpr auto red_on_black =
	vga_text_make_color(VGATextColors::Red, VGATextColors::Black);
constexpr auto green_on_black =
	vga_text_make_color(VGATextColors::Green, VGATextColors::Black);
constexpr auto white_on_black =
	vga_text_make_color(VGATextColors::White, VGATextColors::Black);

#if CONFIG_ARCH == ARCH_x86_64
extern "C" int early_init()
{
	vga_text_clear();

	VGAText vga_text;
	ArchInfo arch_info;

	if (!try_x86_cpuid_verbose(arch_info, vga_text)) {
		vga_text.set_color(red_on_black);
		vga_text.puts("Can't boot 64bit image.\r\n");
		return -1;
	}

	print_vendor_info(vga_text, arch_info);

	if (kstd::test(arch_info.ext_feature_flags,ExtFeatureFlags::LongMode)) {
		vga_text.set_color(green_on_black);
		vga_text.puts("Long mode available.\r\n");
	} else {
		vga_text.set_color(red_on_black);
		vga_text.puts(
			"Long mode unavailable. Can't boot 64bit image.\r\n");
		return -1;
	}


	KernelMemLayout mem_layout = {
		.text = {
			.start_vma = __ldconfig__KERNEL_TEXT_START_VMA,
			.start_lma = __ldconfig__KERNEL_TEXT_START_LMA,
			.size = __ldconfig__KERNEL_TEXT_SIZE,
		},
		.bss = {
			.start_vma = __ldconfig__KERNEL_BSS_START_VMA,
			.start_lma = __ldconfig__KERNEL_BSS_START_LMA,
			.size = __ldconfig__KERNEL_BSS_SIZE,
		},
		.rodata = {
			.start_vma = __ldconfig__KERNEL_RODATA_START_VMA,
			.start_lma = __ldconfig__KERNEL_RODATA_START_LMA,
			.size = __ldconfig__KERNEL_RODATA_SIZE,
		},
		.data = {
			.start_vma = __ldconfig__KERNEL_DATA_START_VMA,
			.start_lma = __ldconfig__KERNEL_DATA_START_LMA,
			.size = __ldconfig__KERNEL_DATA_SIZE,
		},
	};

	uintptr_t map_location = (uintptr_t)__ldconfig__KERNEL_IMAGE_END_LMA;
	new ((void *)map_location) PageTable;
	constexpr auto PAGE_SHIFT = page_size_shifts[0];
	constexpr auto PAGE_MASK = (1 << PAGE_SHIFT) - 1;
	if (map_location & PAGE_MASK)
		map_location = ((map_location >> PAGE_SHIFT) + 1) << PAGE_SHIFT;

	PageTable *page_table = reinterpret_cast<PageTable *>(map_location);
	map_location += PageTable::size;

	auto e = map_identity_pages_preceding_kernel(map_location, page_table);
	if (e != LocalErr::None) {
		vga_text.set_color(red_on_black);
		vga_text.puts(
			"Failed to identity map pre kernel start memory.");
		return -1;
	}

	e = map_kernel_memory(mem_layout, map_location, page_table);
	if (e != LocalErr::None) {
		vga_text.set_color(red_on_black);
		vga_text.puts("Failed to map kernel memory.");
		return -1;
	}

	set_curr_pt_ptr((PhysAddr)page_table);
	// TODO: check the cpuid features
	enable_paging();

	return 0;
}
#else
int early_init()
{
	// unimplemented
	return -2;
}
#endif

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

static LocalErr map_identity_pages_preceding_kernel(uintptr_t &map_location,
		PageTable *page_table)
{
	const size_t mem_size = (uintptr_t)__ldconfig__KERNEL_TEXT_START_LMA-0;
	auto e = page_table->map_memory__no_mm(0, 0, mem_size,
			PageEntryFlags::Global | PageEntryFlags::Supervisor
			| PageEntryFlags::WriteAllowed,
			map_location, map_location + 0x10000000);
	if (e != PageMapErr::None)
		return LocalErr::PreKernelIdentityMapFail;

	return LocalErr::None;
}

static LocalErr map_kernel_memory(const KernelMemLayout &mem_layout,
	uintptr_t &map_location, PageTable *page_table)
{
	PageMapErr e;

	e = page_table->map_memory__no_mm(
			(LineAddr)mem_layout.text.start_vma,
			(PhysAddr)mem_layout.text.start_lma,
			mem_layout.text.size,
			PageEntryFlags::Global | PageEntryFlags::Supervisor,
			map_location, map_location + 0x10000000);
	if (e != PageMapErr::None)
		return LocalErr::KernelMapFail;

	e = page_table->map_memory__no_mm(
			(LineAddr)mem_layout.bss.start_vma,
			(PhysAddr)mem_layout.bss.start_lma,
			mem_layout.bss.size,
			PageEntryFlags::Global
			| PageEntryFlags::Supervisor
			| PageEntryFlags::WriteAllowed
			| PageEntryFlags::ExecuteDisabled,
			map_location, map_location + 0x10000000);
	if (e != PageMapErr::None)
		return LocalErr::KernelMapFail;

	e = page_table->map_memory__no_mm(
			(LineAddr)mem_layout.rodata.start_vma,
			(PhysAddr)mem_layout.rodata.start_lma,
			mem_layout.rodata.size,
			PageEntryFlags::Global | PageEntryFlags::Supervisor
			| PageEntryFlags::ExecuteDisabled,
			map_location, map_location + 0x10000000);
	if (e != PageMapErr::None)
		return LocalErr::KernelMapFail;

	e = page_table->map_memory__no_mm(
			(LineAddr)mem_layout.data.start_vma,
			(PhysAddr)mem_layout.data.start_lma,
			mem_layout.data.size,
			PageEntryFlags::Global | PageEntryFlags::Supervisor
			| PageEntryFlags::WriteAllowed
			| PageEntryFlags::ExecuteDisabled,
			map_location, map_location + 0x10000000);
	if (e != PageMapErr::None)
		return LocalErr::KernelMapFail;

	return LocalErr::None;
}

} // x86
