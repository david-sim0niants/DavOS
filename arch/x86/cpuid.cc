#include <x86/cpuid.h>
#include <stdint.h>
#include <string.h>


#define CPUID() asm ( 	"cpuid" 			\
	: "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) 	\
	: "a" (leaf), "c"(subleaf));

namespace x86 {
/* Get standard CPUID info. */
static void cpuid__standard(struct ArchInfo& info);
/* Get extended CPUID info. */
static void cpuid__extended(struct ArchInfo& info);

bool check_cpuid_presence()
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

void cpuid__assume_cpuid_present(ArchInfo& info)
{
	cpuid__standard(info);
	cpuid__extended(info);
}

bool cpuid(ArchInfo& info)
{
	if (!check_cpuid_presence())
		return false;
	cpuid__assume_cpuid_present(info);
	return true;
}

static void cpuid__standard(ArchInfo& info)
{
	uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0;
	uint32_t leaf = 0, subleaf = 0;

	CPUID();

	memcpy(info.vendor_id, &ebx, 4);
	memcpy(info.vendor_id + 4, &edx, 4);
	memcpy(info.vendor_id + 8, &ecx, 4);

	uint32_t max_standard_leaf = eax;
	info.feature_flags = FeatureFlags::None;

	leaf = 0x01;
	if (leaf > max_standard_leaf)
		return;
	CPUID();
	info.feature_flags = info.feature_flags
		| kstd::switch_flag(FeatureFlags::PSE, edx & (1 << 3))
		| kstd::switch_flag(FeatureFlags::PAE, edx & (1 << 6))
		| kstd::switch_flag(FeatureFlags::PGE, edx & (1 << 13));

	leaf = 0x07;
	if (leaf > max_standard_leaf)
		return;
	CPUID();
	info.feature_flags = info.feature_flags
		| kstd::switch_flag(FeatureFlags::SMEP, ebx & (1 << 7))
		| kstd::switch_flag(FeatureFlags::SMAP, ebx & (1 << 20))
		| kstd::switch_flag(FeatureFlags::LA57, ecx & (1 << 16));
}

static void cpuid__extended(ArchInfo& info)
{
	uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0;
	uint32_t leaf = 0x80000000, subleaf = 0;

	CPUID();

	uint32_t max_extended_leaf = eax;
	info.ext_feature_flags = ExtFeatureFlags::None;

	leaf = 0x80000001;
	if (leaf > max_extended_leaf)
		return;
	CPUID();
	info.ext_feature_flags = info.ext_feature_flags
		| kstd::switch_flag(ExtFeatureFlags::NX, edx & (1<<20))
		| kstd::switch_flag(ExtFeatureFlags::Page_1Gb, (edx & (1<<26)))
		| kstd::switch_flag(ExtFeatureFlags::LongMode, (edx & (1<<29)));

	leaf = 0x80000008;
	if (leaf > max_extended_leaf)
		return;

	info.max_phy_addr = eax & 0xFF;
	info.max_lin_addr = (eax & 0xFF00) >> 8;
}

} // namespace x86
