#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include <config.h>

#include <x86/vga_text.h>
#include <x86/paging/page_table_level_4.h>

static bool check_cpuid_presence();
static bool check_long_mode_presence();


int arch_init()
{
	vga_text_clear();

	struct vga_text vga_text;
	vga_text_init(&vga_text);

#if CONFIG_ARCH == ARCH_x86_64
	vga_text_print(&vga_text, "Checking CPUID.\r\n");
	if (check_cpuid_presence()) {
		vga_text_set_color(&vga_text, VGA_TEXT_COLOR_GREEN);
		vga_text_print(&vga_text, "CPUID available.\r\n");
	} else {
		vga_text_set_color(&vga_text, VGA_TEXT_COLOR_RED);
		vga_text_print(&vga_text,
			"CPUID unavailable. Can't boot 64bit image.");
		return 1;
	}

	if (check_long_mode_presence()) {
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


static bool check_cpuid_presence()
{
	uint32_t curr_flags, prev_flags;
	asm ( 	".equ ID_BIT, 1 << 21 	\n"
		"pushf 			\n"
		"pop %0 		\n"
		"mov %0, %1 	 	\n"
		"xor $ID_BIT, %0 	\n"
		"push %0 		\n"
		"popf 			\n"
		"pushf  		\n"
		"pop %0 		\n"
		: "=r" (curr_flags), "=r" (prev_flags)
	);

	return curr_flags != prev_flags;
}


static bool check_long_mode_presence()
{
	uint32_t eax_value;
	asm ( 	"mov $0x80000000, %%eax \n"
		"cpuid 			\n"
		"mov %%eax, %0 		\n"
		: "+r" (eax_value)
	);
	if (eax_value < 0x80000001)
		return false;

	uint32_t edx_value;
	asm ( 	"mov $0x80000001, %%eax \n"
		"cpuid 			\n"
		"mov %%edx, %0 		\n"
		: "+r" (edx_value)
	);

#define LONG_MODE_BIT 1 << 29
	return !!(edx_value & LONG_MODE_BIT);
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
	struct main_kernel_mem_layout *mem_layout, void *mapping_location)
{
}
