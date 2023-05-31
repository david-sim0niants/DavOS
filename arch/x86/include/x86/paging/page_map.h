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

enum PageEntryFlags {
	PAGE_ENTRY_NONE = 0x0,
	PAGE_ENTRY_WRITE_ALLOWED = 0x1,
	PAGE_ENTRY_SUPERVISOR = PAGE_ENTRY_WRITE_ALLOWED << 1,
	PAGE_ENTRY_GLOBAL = PAGE_ENTRY_SUPERVISOR << 1,
	PAGE_ENTRY_EXECUTE_DISABLED = PAGE_ENTRY_GLOBAL << 1,
};


template<int pml> inline unsigned get_pte_idx(LineAddr linaddr)
{
	static_assert(pml >= 0 && pml <= MAX_PAGE_MAP_LEVEL,
		"Invalid page map level.");

	constexpr auto BEG_BIT_LOC = []() constexpr
	{
		if constexpr (pml == 0)
			return 0;
		else
			return PageTableEntry_<pml>::CONTROLLED_BITS;
	}();
	constexpr auto BEG_BIT = LineAddr(1) << BEG_BIT_LOC;

	constexpr auto END_BIT = []() constexpr -> LineAddr
	{
		if constexpr (pml == MAX_PAGE_MAP_LEVEL)
			return 0;
		else
			return LineAddr(1)
				<< PageTableEntry_<pml + 1>::CONTROLLED_BITS;
	}();

	return ((END_BIT - BEG_BIT) & linaddr) >> BEG_BIT_LOC;
}

template<int pml>
class PageTable_ {
public:
	friend class PageTable_<pml + 1>;

	enum class Err {
		NONE = 0, EXISTING_PAGE_MAP, NO_FREE_MEM
	};

	Err map_page__no_mm(LineAddr linaddr, PhysAddr phyaddr, PageSize ps,
		int flags, uintptr_t &free_mem_beg, uintptr_t free_mem_end);

	Err map_page_range__no_mm(LineAddr linaddr, PhysAddr phyaddr,
		size_t range_size, int flags,
		uintptr_t free_mem_beg, uintptr_t free_mem_end);

	static constexpr auto NUM_ENTRIES = 1<<PageTableEntry_<pml>::INDEX_BITS;
	static constexpr auto SIZE = NUM_ENTRIES * sizeof(PageTableEntry_<pml>);

private:
	template<PageSize page_size>
	Err map_page__no_mm_internal(LineAddr linaddr, PhysAddr phyaddr,
		int flags, uintptr_t &free_mem_beg, uintptr_t free_mem_end);

	PageTableEntry_<pml> entries[NUM_ENTRIES] = {};
};

using PageTable = PageTable_<MAX_PAGE_MAP_LEVEL>;
using PageTableEntry = PageTableEntry_<MAX_PAGE_MAP_LEVEL>;

}

#endif
