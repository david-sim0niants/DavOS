#ifndef _x86_PAGING__PAGE_TABLE_ENTRY_H__
#define _x86_PAGING__PAGE_TABLE_ENTRY_H__

#include <x86/config.h>
#include <x86/addressing.h>

namespace x86 {

constexpr int MAX_PAGE_MAP_LEVEL =
#if CONFIG_x86_PAGE_MAP_LEVEL == x86_PAGE_MAP_LEVEL_2
	2
#elif CONFIG_x86_PAGE_MAP_LEVEL == x86_PAGE_MAP_LEVEL_3_PAE
	3
#elif CONFIG_x86_PAGE_MAP_LEVEL == x86_PAGE_MAP_LEVEL_4
	4
#elif CONFIG_x86_PAGE_MAP_LEVEL == x86_PAGE_MAP_LEVEL_5
	5
#else
	0
#endif
;

using PageTableEntryValue =
#if CONFIG_x86_PAGE_MAP_LEVEL == x86_PAGE_MAP_LEVEL_2
	uint32_t
#else
	uint64_t
#endif
;


template<int pml>
struct PageTableEntry_ {
	static_assert(pml <= MAX_PAGE_MAP_LEVEL, "Page map level is too high.");
	static_assert(pml > 0,
		"Page table entry can't have a page map level lower than 1.");

	bool is_present();
	bool is_write_allowed();
	bool is_supervisor();
	bool maps_page();
	bool maps_page_table();
	bool is_global();
	bool is_execute_disabled();

	void set_present(bool present);
	void set_write_allowed(bool write_allowed);
	void set_user_or_supervisor(bool supervisor);
	void map_page(PhysAddr page_addr, bool global);
	void map_page_table(PhysAddr pt_addr);
	void set_execute_disabled(bool execute_disable);

	PhysAddr get_page_addr();
	PhysAddr get_page_table_addr();

	static const unsigned INDEX_BITS;
	static const unsigned CONTROLLED_BITS;
	PageTableEntryValue value;
};

}

#endif
