#ifndef x86__CPUID_H__
#define x86__CPUID_H__

#include <stdbool.h>

/* x86 standard feature flags. */
enum x86_FeatureFlags {
	x86_FEATURE_NONE = 	0x0,
	/* Page-size extension for 32bit paging. */
	x86_FEATURE_PSE = 	0x1,
	/* Physical-address extension. */
	x86_FEATURE_PAE = 	x86_FEATURE_PSE << 1,
	/* Supervisor-mode execution prevention. */
	x86_FEATURE_SMEP = 	x86_FEATURE_PAE << 1,
	/* Supervisor-mode access prevention. */
	x86_FEATURE_SMAP = 	x86_FEATURE_SMEP << 1,
	/* 57bit linear addresses and 5-level paging. */
	x86_FEATURE_LA57 = 	x86_FEATURE_SMAP << 1,
};

/* x86 extended feature flags. */
enum x86_ExtFeatureFlags {
	x86_EXT_FEATURE_NONE = 		0x0,
	/* Execute disable. */
	x86_EXT_FEATURE_NX = 		0x1,
	/* Support for 1Gb pages. */
	x86_EXT_FEATURE_PAGE_1GB = 	x86_EXT_FEATURE_NX << 1,
	/* Long mode support. */
	x86_EXT_FEATURE_LONG_MODE = 	x86_EXT_FEATURE_PAGE_1GB << 1,
};

/* x86 current architecture info. */
struct x86_ArchInfo {
	char vendor_id[12]; /* Vendor ID */
	int feature_flags; /* Standard feature flags. */
	int ext_feature_flags; /* Extended feature flags. */
	unsigned short max_phy_addr; /* Max number of physical address bits. */
	unsigned short max_lin_addr; /* Max number of linear address bits. */
};


/* Check if CPUID is present. */
bool x86_check_cpuid_presence();
/* Get current x86 architecture info from CPUID. */
void x86_cpuid(struct x86_ArchInfo *info);

#endif
