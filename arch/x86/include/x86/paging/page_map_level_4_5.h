#ifndef x86_PAGING__PAGE_MAP_LEVEL_4_5_H__
#define x86_PAGING__PAGE_MAP_LEVEL_4_5_H__

#include <stdint.h>

#define PAGE_ENTRY_INDEX_BITS 9
#define NR_PAGE_ENTRIES (1 << PAGE_ENTRY_INDEX_BITS)

typedef uint64_t PEntry_t;
typedef PEntry_t PTable_t[NR_PAGE_ENTRIES];

typedef uint64_t PDEntry_t;
typedef PDEntry_t PDTable_t[NR_PAGE_ENTRIES];

typedef uint64_t PDPEntry_t;
typedef PDPEntry_t PDPTable_t[NR_PAGE_ENTRIES];

typedef uint64_t PML4Entry_t;
typedef PML4Entry_t PML4Table_t[NR_PAGE_ENTRIES];

typedef uint64_t PML5Entry_t;
typedef PML5Entry_t PML5Table_t[NR_PAGE_ENTRIES];


struct LinearAddress_within_4KbPage {
	uint64_t offset : 12;
	uint64_t page : PAGE_ENTRY_INDEX_BITS;
	uint64_t page_directory : PAGE_ENTRY_INDEX_BITS;
	uint64_t page_directory_pointer : PAGE_ENTRY_INDEX_BITS;
	uint64_t page_map_level_4 : PAGE_ENTRY_INDEX_BITS;
	uint64_t page_map_level_5 : PAGE_ENTRY_INDEX_BITS;
};

struct LinearAddress_within_2MbPage {
	uint64_t offset : 21;
	uint64_t page_directory : PAGE_ENTRY_INDEX_BITS;
	uint64_t page_directory_pointer : PAGE_ENTRY_INDEX_BITS;
	uint64_t page_map_level_4 : PAGE_ENTRY_INDEX_BITS;
	uint64_t page_map_level_5 : PAGE_ENTRY_INDEX_BITS;
};

struct LinearAddress_within_1GbPage {
	uint64_t offset : 30;
	uint64_t page_directory_pointer : PAGE_ENTRY_INDEX_BITS;
	uint64_t page_map_level_4 : PAGE_ENTRY_INDEX_BITS;
	uint64_t page_map_level_5 : PAGE_ENTRY_INDEX_BITS;
};

#endif
