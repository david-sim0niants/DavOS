#ifndef x86__CPUID_H__
#define x86__CPUID_H__


namespace x86 {

/* x86 standard feature flags. */
enum FeatureFlags {
	FEATURE_NONE = 	0x0,
	/* Page-size extension for 32bit paging. */
	FEATURE_PSE = 	0x1,
	/* Physical-address extension. */
	FEATURE_PAE = 	FEATURE_PSE << 1,
	/* Supervisor-mode execution prevention. */
	FEATURE_SMEP = 	FEATURE_PAE << 1,
	/* Supervisor-mode access prevention. */
	FEATURE_SMAP = 	FEATURE_SMEP << 1,
	/* 57bit linear addresses and 5-level paging. */
	FEATURE_LA57 = 	FEATURE_SMAP << 1,
};

/* x86 extended feature flags. */
enum ExtFeatureFlags {
	EXT_FEATURE_NONE = 	0x0,
	/* Execute disable. */
	EXT_FEATURE_NX = 	0x1,
	/* Support for 1Gb pages. */
	EXT_FEATURE_PAGE_1GB = 	EXT_FEATURE_NX << 1,
	/* Long mode support. */
	EXT_FEATURE_LONG_MODE = EXT_FEATURE_PAGE_1GB << 1,
};

/* x86 current architecture info. */
struct ArchInfo {
	char vendor_id[12]; /* Vendor ID */
	int feature_flags; /* Standard feature flags. */
	int ext_feature_flags; /* Extended feature flags. */
	unsigned short max_phy_addr; /* Max number of physical address bits. */
	unsigned short max_lin_addr; /* Max number of linear address bits. */
};


/* Check if CPUID is present. */
bool check_cpuid_presence();
/* Get current x86 architecture info from CPUID.
 * Returns true if CPUID is available, false otherwise. */
bool cpuid(ArchInfo &info);
/* Get current x86 architecture info from CPUID.
 * Doesn't check CPUID presence. */
void cpuid__assume_cpuid_present(ArchInfo &info);

}

#endif
