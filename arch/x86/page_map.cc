#include <x86/paging/page_map.h>


namespace x86 {

template<int pml> PageTable_<pml>::PageTable_(PageTableEntry_<pml> *entries)
	: entries(entries)
{
}

template<int pml>
void PageTable_<pml>::map_page__no_mm(LineAddr linaddr, PhysAddr phyaddr,
	int pg_prot, void *free_mem_beg, void *free_mem_end)
{
	// TODO: implement this
}

}
