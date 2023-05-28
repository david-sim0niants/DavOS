#ifndef _x86_PAGING__PAGE_MAP_H__
#define _x86_PAGING__PAGE_MAP_H__


#include <x86/config.h>
#include <x86/addressing.h>
#include <stddef.h>


#if CONFIG_ARCH == ARCH_x86_64
	#if CONFIG_x86_PAGE_MAP_LEVEL == x86_PAGE_MAP_LEVEL_4 || \
		CONFIG_x86_PAGE_MAP_LEVEL == x86_PAGE_MAP_LEVEL_5
		#include <x86/paging/page_map_level_4_5.h>
	#else
		#error "At least one kind of paging map level should be specified for x86_64 architecture."
	#endif
#else
	#if (CONFIG_x86_PAGE_MAP_LEVEL == x86_PAGE_MAP_LEVEL_2) || \
		(CONFIG_x86_PAGE_MAP_LEVEL == x86_PAGE_MAP_LEVEL_3_PAE)
		#if CONFIG_x86_PAGE_MAP_LEVEL &x86_PAGE_MAP_LEVEL_2
			#include <x86/paging/page_map_level_2.h>
		#endif
		#if CONFIG_x86_PAGE_MAP_LEVEL == x86_PAGE_MAP_LEVEL_3_PAE
			#include <x86/paging/page_map_level_3_PAE.h>
		#endif
	#else
		#error "At least one kind of paging map level should be specified for i386 architecture."
	#endif
#endif

namespace x86 {

enum PageProtFlags {
	PAGE_PROT_READ = 0x0, 
	PAGE_PROT_WRITE = 0x1,
	PAGE_PROT_EXEC = 0x2,
};



template<int pml> inline unsigned get_pte_idx(LineAddr linaddr)
{
	static_assert(pml >= 0 && pml <= MAX_PAGE_MAP_LEVEL,
		"Invalid page map level.");

	constexpr auto BEG_BIT_LOC = pml == 0 ?
		0 : PageTableEntry_<pml>::CONTROLLED_BITS;
	constexpr auto BEG_BIT = 1 << BEG_BIT_LOC;
	constexpr auto END_BIT = pml == MAX_PAGE_MAP_LEVEL ?
		0 : (1 << PageTableEntry_<pml + 1>::CONTROLLED_BITS);

	return ((END_BIT - BEG_BIT) & linaddr) >> BEG_BIT_LOC;
}


template<int pml>
class PageTable_ {
public:
	explicit PageTable_(PageTableEntry_<pml> *entries);

	void map_page__no_mm(LineAddr linaddr, PhysAddr phyaddr, int pg_prot,
		void *free_mem_beg, void *free_mem_end);

private:
	PageTableEntry_<pml> *entries;
	static constexpr auto NUM_ENTRIES = 1<<PageTableEntry_<pml>::INDEX_BITS;
};

using PageTable = PageTable_<MAX_PAGE_MAP_LEVEL>;
using PageTableEntry = PageTableEntry_<MAX_PAGE_MAP_LEVEL>;

void map_pages__no_mm(void *linaddr, void *phyaddr, size_t size, int pg_prot,
	void *pt_free_mem_ptr, size_t pt_free_mem_len);

}

#endif
