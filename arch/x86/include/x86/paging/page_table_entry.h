#ifndef _x86_PAGING__PAGE_TABLE_ENTRY_H__
#define _x86_PAGING__PAGE_TABLE_ENTRY_H__

#include <x86/config.h>
#include <x86/addressing.h>

namespace x86 {

constexpr int max_page_map_level =
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
	static_assert(pml <= max_page_map_level, "Page map level is too high.");
	static_assert(pml > 0,
		"Page table entry can't have a page map level lower than 1.");

	bool is_present() const;
	bool is_write_allowed() const;
	bool is_supervisor() const;
	bool maps_page() const;
	bool maps_page_table() const;
	bool is_global() const;
	bool is_execute_disabled() const;

	void set_present(bool present);
	void set_write_allowed(bool write_allowed);
	void set_user_or_supervisor(bool supervisor);
	void map_page(PhysAddr page_addr, bool global);
	void map_page_table(PhysAddr pt_addr);
	void set_execute_disabled(bool execute_disable);

	PhysAddr get_page_addr() const;
	PhysAddr get_page_table_addr() const;

	static const unsigned index_bits;
	static const unsigned controlled_bits;
	PageTableEntryValue value;
};

}

#endif
