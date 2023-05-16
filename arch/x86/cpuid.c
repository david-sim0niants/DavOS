#include <x86/cpuid.h>
#include <stdint.h>
#include <string.h>


#define CPUID() 						\
	asm ( 	"cpuid" 					\
	      : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) 	\
	      : "a" (leaf), "c"(subleaf));

/* Get standard CPUID info. */
static void x86_cpuid__standard(struct x86_ArchInfo *info);
/* Get extended CPUID info. */
static void x86_cpuid__extended(struct x86_ArchInfo *info);

bool x86_check_cpuid_presence()
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

void x86_cpuid(struct x86_ArchInfo *info)
{
	x86_cpuid__standard(info);
	x86_cpuid__extended(info);
}

static void x86_cpuid__standard(struct x86_ArchInfo *info)
{
	uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0;
	uint32_t leaf = 0, subleaf = 0;

	CPUID();

	memcpy(info->vendor_id, &ebx, 4);
	memcpy(info->vendor_id + 4, &edx, 4);
	memcpy(info->vendor_id + 8, &ecx, 4);

	uint32_t max_standard_leaf = eax;
	info->feature_flags = 0;

	leaf = 0x01;
	if (leaf > max_standard_leaf)
		return;
	CPUID();
	info->feature_flags |= x86_FEATURE_PSE * (!!(edx & (1 << 3)));
	info->feature_flags |= x86_FEATURE_PAE * (!!(edx & (1 << 6)));

	leaf = 0x07;
	if (leaf > max_standard_leaf)
		return;
	CPUID();
	info->feature_flags |= x86_FEATURE_SMEP * (!!(ebx & (1 << 7)));
	info->feature_flags |= x86_FEATURE_SMAP * (!!(ebx & (1 << 20)));
	info->feature_flags |= x86_FEATURE_LA57 * (!!(ecx & (1 << 16)));
}

static void x86_cpuid__extended(struct x86_ArchInfo *info)
{
	uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0;
	uint32_t leaf = 0x80000000, subleaf = 0;

	CPUID();

	uint32_t max_extended_leaf = eax;
	info->ext_feature_flags = 0;

	leaf = 0x80000001;
	if (leaf > max_extended_leaf)
		return;
	CPUID();
	info->ext_feature_flags |= x86_EXT_FEATURE_NX * (!!(edx & (1 << 20)));
	info->ext_feature_flags |= x86_EXT_FEATURE_PAGE_1GB * (!!(edx & (1 << 26)));
	info->ext_feature_flags |= x86_EXT_FEATURE_LONG_MODE * (!!(edx & (1 << 29)));

	leaf = 0x80000008;
	if (leaf > max_extended_leaf)
		return;

	info->max_phy_addr = eax & 0xFF;
	info->max_lin_addr = (eax & 0xFF00) >> 8;
}
