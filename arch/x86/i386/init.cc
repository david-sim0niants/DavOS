#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <config.h>
#include <x86/config.h>
#include <ldconfig.h>

#include <x86/cpuid.h>
#include <x86/vga_text.h>
#include <x86/paging/page_map.h>


namespace x86 {

static void print_vendor_info(VGAText &vga_text, ArchInfo &arch_info);
static bool try_x86_cpuid_verbose(ArchInfo &arch_info, VGAText &vga_text);

struct MainKernelSection {
	void *start_vma, *start_lma; size_t size;
};

struct MainKernelMemLayout {
	MainKernelSection text, bss, rodata, data;
};

static void map_main_kernel_pages(const MainKernelMemLayout &mem_layout,
	uintptr_t map_location);


constexpr char RED_ON_BLACK =
	vga_text_make_color(VGA_TEXT_COLOR_RED, VGA_TEXT_COLOR_BLACK);
constexpr char GREEN_ON_BLACK =
	vga_text_make_color(VGA_TEXT_COLOR_GREEN, VGA_TEXT_COLOR_BLACK);
constexpr char WHITE_ON_BLACK =
	vga_text_make_color(VGA_TEXT_COLOR_GREEN, VGA_TEXT_COLOR_BLACK);

#if CONFIG_ARCH == ARCH_x86_64
extern "C" int arch_init()
{
	vga_text_clear();

	VGAText vga_text;
	ArchInfo arch_info;

	if (!try_x86_cpuid_verbose(arch_info, vga_text)) {
		vga_text.set_color(RED_ON_BLACK);
		vga_text.puts("Can't boot 64bit image.\r\n");
		return -1;
	}

	print_vendor_info(vga_text, arch_info);

	if (arch_info.ext_feature_flags & EXT_FEATURE_LONG_MODE) {
		vga_text.set_color(GREEN_ON_BLACK);
		vga_text.puts("Long mode available.\r\n");
	} else {
		vga_text.set_color(RED_ON_BLACK);
		vga_text.puts(
			"Long mode unavailable. Can't boot 64bit image.\r\n");
		return -1;
	}


	MainKernelMemLayout mem_layout = {
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

	constexpr auto PAGE_TABLE_SIZE_BITS = 12;
	uintptr_t map_location = (uintptr_t)__ldconfig__KERNEL_IMAGE_END_LMA;

	if ((map_location & ((1 << PAGE_TABLE_SIZE_BITS) - 1)) != 0)
		map_location = ((map_location >> 12) + 1) << 12;

	map_main_kernel_pages(mem_layout, map_location);

	return 0;
}
#else
	int arch_init()
	{
		// unimplemented
		return -2;
	}
#endif

static bool try_x86_cpuid_verbose(ArchInfo &arch_info, VGAText &vga_text)
{
	vga_text.puts("Checking CPUID.\r\n");

	if (cpuid(arch_info)) {
		vga_text.set_color(GREEN_ON_BLACK);
		vga_text.puts("CPUID available.\r\n");
		return true;
	} else {
		vga_text.set_color(VGA_TEXT_COLOR_RED);
		vga_text.puts("CPUID unavailable.\r\n");
		return false;
	}
}

static void print_vendor_info(VGAText &vga_text, ArchInfo &arch_info)
{
	vga_text.set_color(WHITE_ON_BLACK);

	vga_text.puts("Processor vendor: ");

	const size_t vendor_id_len = sizeof(arch_info.vendor_id);
	vga_text.write_buffer(arch_info.vendor_id, vendor_id_len);
	vga_text.puts("\r\n");
}


static void map_main_kernel_pages(const MainKernelMemLayout &mem_layout,
	uintptr_t map_location)
{
	PageTable *page_table = reinterpret_cast<PageTable *>(map_location);
	map_location += PageTable::SIZE;

	page_table->map_page__no_mm((PhysAddr)mem_layout.text.start_lma,
		(LineAddr)mem_layout.text.start_vma, PageSize::PS_4Kb,
		PAGE_ENTRY_GLOBAL | PAGE_ENTRY_SUPERVISOR,
		map_location, map_location + 0x10000000);
}

} // x86
