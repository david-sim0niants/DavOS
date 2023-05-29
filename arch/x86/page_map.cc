#include <x86/paging/page_map.h>


namespace x86 {

template<int pml> PageTable_<pml>::PageTable_(PageTableEntry_<pml> *entries)
	: entries(entries)
{
}

template<int pml> template<PageSize page_size> typename PageTable_<pml>::
Err PageTable_<pml>::map_page__no_mm(LineAddr linaddr, PhysAddr phyaddr,
	int flags, char *&free_mem_beg, char *free_mem_end)
{
	static constexpr
	auto CONTROLLED_MEM_REGION = 1 << PageTableEntry_<pml>::CONTROLLED_BITS;

	static_assert(page_size <= CONTROLLED_MEM_REGION,
		"Page size larger than the controlled memory region at the "
		"current page map level.\n"
		"NOTE: it was probably smaller than the controlled memory "
		"region at the above page map level which means it's an "
		"invalid value.");

	PageTableEntry_<pml> &entry = entries[get_pte_idx<pml>(linaddr)];

	if constexpr (page_size == CONTROLLED_MEM_REGION) {
		if (entry.is_present())
			return Err::EXISTING_PAGE_MAP;
		else
			entry.set_present(true);
		entry.map_page(phyaddr, flags & PAGE_ENTRY_GLOBAL);
	} else {
		if (entry.maps_page())
			return Err::EXISTING_PAGE_MAP;
		PhysAddr next_pt_addr;
		if (entry.maps_page_table()) {
			next_pt_addr = entry.get_page_table_addr();
		} else {
			free_mem_beg = free_mem_beg << /* 12bits ? */;
		}
	}

	entry.set_write_allowed(flags & PAGE_ENTRY_WRITE_ALLOWED);
	entry.set_user_or_supervisor(flags & PAGE_ENTRY_SUPERVISOR);
	entry.set_execute_disabled(flags & PAGE_ENTRY_EXECUTE_DISABLED);

	return Err::NONE;
}

template<int pml> typename PageTable_<pml>::
Err PageTable_<pml>::map_page_range__no_mm(LineAddr linaddr, PhysAddr phyaddr,
	size_t range_size, int flags, char *free_mem_beg, char *free_mem_end)
{
}

template class PageTable_<MAX_PAGE_MAP_LEVEL>;

}
