#include <stdbool.h>
#include <stdint.h>

#include <arch/vga_text.h>

static bool check_cpuid();

void arch_init()
{
	vga_text_clear();

	struct vga_text vga_text;
	vga_text_init(&vga_text);

	vga_text_print(&vga_text, "Checking CPUID.\r\n");
	if (check_cpuid()) {
		vga_text_print(&vga_text, "CPUID available.\r\n");
	}
}


static bool check_cpuid()
{
	uint32_t curr_flags, prev_flags;
	asm ( 	"pushf 			\n"
		"pop %0 		\n"
		"mov %0, %1 	 	\n"
		"xor $1 << 21, %0 	\n"
		"push %0 		\n"
		"popf 			\n"
		"pushf  		\n"
		: "=r" (curr_flags), "=r" (prev_flags)
	);

	return curr_flags == prev_flags;
}
