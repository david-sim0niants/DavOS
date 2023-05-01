#include <stdbool.h>
#include <stdint.h>

static bool check_cpuid();

void arch_init()
{
}


static bool check_cpuid()
{
	uint32_t curr_flags, prev_flags;
	asm ( 	"pushf 			\n"
		"pop %%eax 		\n"
		"mov %%eax, %%ecx 	\n"
		"xor $1 << 21, %%eax 	\n"
		"push %%eax 		\n"
		"popf 	 		\n"
		"pushf  		\n"
		: "=a" (curr_flags), "=c" (prev_flags)
	);
}
