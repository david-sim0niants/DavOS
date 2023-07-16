#ifndef _x86__CPUID_H__
#define _x86__CPUID_H__

#include <kstd/enum.h>

namespace x86 {

/* x86 standard feature flags. */
enum class FeatureFlags {
	None = 	0x0,
	/* Page-size extension for 32bit paging. */
	PSE = 	0x1,
	/* Physical-address extension. */
	PAE = 	PSE << 1,
	PGE = 	PAE << 1,
	/* Supervisor-mode execution prevention. */
	SMEP = 	PGE << 1,
	/* Supervisor-mode access prevention. */
	SMAP = 	SMEP << 1,
	/* 57bit linear addresses and 5-level paging. */
	LA57 = 	SMAP << 1,
};
KSTD_DEFINE_ENUM_LOGIC_BITWISE_OPERATORS(FeatureFlags);

/* x86 extended feature flags. */
enum class ExtFeatureFlags {
	None = 		0x0,
	/* Execute disable. */
	NX = 		0x1,
	/* Support for 1Gb pages. */
	Page_1Gb = 	NX << 1,
	/* Long mode support. */
	LongMode = 	Page_1Gb << 1,
};
KSTD_DEFINE_ENUM_LOGIC_BITWISE_OPERATORS(ExtFeatureFlags);

/* x86 current architecture info. */
struct ArchInfo {
	char vendor_id[12]; /* Vendor ID */
	FeatureFlags feature_flags; /* Standard feature flags. */
	ExtFeatureFlags ext_feature_flags; /* Extended feature flags. */
	unsigned short max_phy_addr; /* Max number of physical address bits. */
	unsigned short max_lin_addr; /* Max number of linear address bits. */
};


/* Check if CPUID is present. */
bool check_cpuid_presence();
/* Get current x86 architecture info from CPUID.
 * Returns true if CPUID is available, false otherwise. */
bool cpuid(ArchInfo& info);
/* Get current x86 architecture info from CPUID.
 * Doesn't check CPUID presence. */
void cpuid__assume_cpuid_present(ArchInfo& info);

}

#endif
