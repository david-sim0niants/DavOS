#include <x86/paging/page_map.h>


namespace x86 {

template<int pml>
void PageTable<pml>::map_page__no_mm(LineAddr line_pn, PhysAddr phys_pn,
	int pg_prot, void *free_mem_beg, void *free_mem_end)
{
}

}
