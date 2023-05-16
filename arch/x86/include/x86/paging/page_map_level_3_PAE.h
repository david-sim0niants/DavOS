#ifndef x86_PAGING__PAGE_MAP_LEVEL_3_PAE_H__
#define x86_PAGING__PAGE_MAP_LEVEL_3_PAE_H__

#include <stdint.h>

#define PAE_PAGE_ENTRY_INDEX_BITS 9
#define PAE_NR_PAGE_ENTRIES (1 << PAE_PAGE_ENTRY_INDEX_BITS)

#define PAE_PDP_ENTRY_INDEX_BITS 2
#define PAE_NR_PDP_ENTRIES (1 << PAE_PDP_ENTRY_INDEX_BITS)

typedef uint32_t PAE_PEntry_t;
typedef PAE_PEntry_t PAE_PTable_t[PAE_NR_PAGE_ENTRIES];

typedef uint32_t PAE_PDEntry_t;
typedef PAE_PDEntry_t PAE_PDTable_t[PAE_NR_PAGE_ENTRIES];

typedef uint32_t PAE_PDPEntry_t;
typedef PAE_PDPEntry_t PAE_PDPTable_t[PAE_NR_PDP_ENTRIES]; 


struct PAE_LinearAddress_within_4KbPage {
	uint32_t offset : 12;
	uint32_t page : PAE_PAGE_ENTRY_INDEX_BITS;
	uint32_t page_directory : PAE_PAGE_ENTRY_INDEX_BITS;
	uint32_t page_directory_pointer : PAE_PDP_ENTRY_INDEX_BITS;
};

struct PAE_LinearAddress_within_2MbPage {
	uint32_t offset : 21;
	uint32_t page_directory : PAE_PAGE_ENTRY_INDEX_BITS;
	uint32_t page_directory_pointer : PAE_PDP_ENTRY_INDEX_BITS;
};

#endif
