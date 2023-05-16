#ifndef x86_PAGING__PAGE_MAP_LEVEL_2_H__
#define x86_PAGING__PAGE_MAP_LEVEL_2_H__

#include <stdint.h>

#define PAGE_ENTRY_INDEX_BITS 10
#define NR_PAGE_ENTRIES (1 << PAGE_ENTRY_INDEX_BITS)

typedef uint32_t PEntry_t;
typedef PEntry_t PTable_t[NR_PAGE_ENTRIES];

typedef uint32_t PDEntry_t;
typedef PDEntry_t PDTable_t[NR_PAGE_ENTRIES];


struct LinearAddress_within_4KbPage {
	uint32_t offset : 12;
	uint32_t page : PAGE_ENTRY_INDEX_BITS;
	uint32_t page_directory : PAGE_ENTRY_INDEX_BITS;
};

struct LinearAddress_within_4MbPage {
	uint32_t offset : 22;
	uint32_t page_directory : PAGE_ENTRY_INDEX_BITS;
};


#endif
