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



template<int pml>
class PageTable {
public:
	void map_page__no_mm(LineAddr line_pn, PhysAddr phys_pn, int pg_prot,
		void *free_mem_beg, void *free_mem_end);

private:
	PageTableEntry<pml> *entries;
	static constexpr auto NUM_ENTRIES= 1 << PageTableEntry<pml>::INDEX_BITS;
};


void map_pages__no_mm(void *linaddr, void *phyaddr, size_t size, int pg_prot,
	void *pt_free_mem_ptr, size_t pt_free_mem_len);

}

#endif
