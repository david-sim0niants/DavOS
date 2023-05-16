#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include <config.h>
#include <x86/config.h>
#include <x86/cpuid.h>
#include <x86/vga_text.h>


#if CONFIG_ARCH == ARCH_x86_64
	#if (CONFIG_x86_PAGE_MAP_LEVEL & PAGE_MAP_LEVEL_4) || \
		(CONFIG_x86_PAGE_MAP_LEVEL & PAGE_MAP_LEVEL_5)
		#include <x86/paging/page_map_level_4_5.h>
	#else
		#error "At least one kind of paging map level should be specified for x86_64 architecture."
	#endif
#else
	#if (CONFIG_x86_PAGE_MAP_LEVEL & PAGE_MAP_LEVEL_2) || \
		(CONFIG_x86_PAGE_MAP_LEVEL & PAGE_MAP_LEVEL_3_PAE)
		#if CONFIG_x86_PAGE_MAP_LEVEL & PAGE_MAP_LEVEL_2
			#include <x86/paging/page_map_level_2.h>
		#endif
		#if CONFIG_x86_PAGE_MAP_LEVEL & PAGE_MAP_LEVEL_3_PAE
			#include <x86/paging/page_map_level_3_PAE.h>
		#endif
	#else
		#error "At least one kind of paging map level should be specified for i386 architecture."
	#endif
#endif


static void print_vendor_info(
	struct VGAText *vga_text, struct x86_ArchInfo *arch_info);


int arch_init()
{
	vga_text_clear();

	struct VGAText vga_text;
	vga_text_init(&vga_text);

	vga_text_print(&vga_text, "Checking CPUID.\r\n");
	if (x86_check_cpuid_presence()) {
		vga_text_set_color(&vga_text, VGA_TEXT_COLOR_GREEN);
		vga_text_print(&vga_text, "CPUID available.\r\n");
	} else {
		vga_text_set_color(&vga_text, VGA_TEXT_COLOR_RED);
		vga_text_print(&vga_text,
			"CPUID unavailable. Can't boot 64bit image.");
		return 1;
	}

	struct x86_ArchInfo arch_info;
	x86_cpuid(&arch_info);

	print_vendor_info(&vga_text, &arch_info);

#if CONFIG_ARCH == ARCH_x86_64
	if (arch_info.ext_feature_flags & x86_EXT_FEATURE_LONG_MODE) {
		vga_text_set_color(&vga_text, VGA_TEXT_COLOR_GREEN);
		vga_text_print(&vga_text, "Long mode available.\r\n");
	} else {
		vga_text_set_color(&vga_text, VGA_TEXT_COLOR_RED);
		vga_text_print(&vga_text,
			"Long mode unavailable. Can't boot 64bit image.\r\n");
		return 1;
	}
#endif

	return 0;
}


static void print_vendor_info(
	struct VGAText *vga_text, struct x86_ArchInfo *arch_info)
{
	vga_text_set_color(vga_text, VGA_TEXT_COLOR_WHITE);

	vga_text_print(vga_text, "Processor vendor: ");

	const size_t vendor_id_len = sizeof(arch_info->vendor_id);
	vga_text_write(vga_text, arch_info->vendor_id, vendor_id_len);
	vga_text_print(vga_text, "\r\n");
}


struct main_kernel_mem_layout {
	void *text_start_lma, 	*text_end_lma;
	void *bss_start_lma, 	*bss_end_lma;
	void *rodata_start_lma, *rodata_end_lma;
	void *data_start_lma, 	*data_end_lma;
	unsigned long text_start_vma, 	text_end_vma;
	unsigned long bss_start_vma, 	bss_end_vma;
	unsigned long rodata_start_vma, rodata_end_vma;
	unsigned long data_start_vma, 	data_end_vma;
};


static void map_main_kernel_pages(
	struct main_kernel_mem_layout *mem_layout, void *map_location)
{
}
