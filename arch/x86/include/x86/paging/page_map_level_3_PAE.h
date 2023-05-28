#ifndef x86_PAGING__PAGE_MAP_LEVEL_3_PAE_H__
#define x86_PAGING__PAGE_MAP_LEVEL_3_PAE_H__

#include <stdint.h>
#include <x86/paging/page_table_entry.h>

namespace x86 {

constexpr int PAGE_ENTRY_INDEX_BITS = 9;
constexpr int PDP_ENTRY_INDEX_BITS = 2;

constexpr auto PTE_P_BIT_LOC = 0;
constexpr auto PTE_RW_BIT_LOC = 1;
constexpr auto PTE_US_BIT_LOC = 2;
constexpr auto PTE_PS_BIT_LOC = 7;
constexpr auto PTE_G_BIT_LOC = 8;
constexpr auto PTE_MAKE_PAGE_MASK(auto PAGE_SHIFT)
{
	return ((PhysAddr(1) << 52) - (1 << PAGE_SHIFT));
}
constexpr auto PTE_PT_MASK = PTE_MAKE_PAGE_MASK(12);
constexpr auto PTE_XD_BIT_LOC = 63;

template<int pml> inline bool PageTableEntry_<pml>::is_present()
{
	return !!(value & (1 << PTE_P_BIT_LOC));
}

template<int pml> inline bool PageTableEntry_<pml>::is_write_allowed()
{
	return !!(value & (1 << PTE_RW_BIT_LOC)) || (pml == MAX_PAGE_MAP_LEVEL);
}

template<int pml> inline bool PageTableEntry_<pml>::is_supervisor()
{
	return !!(value & (1 << PTE_US_BIT_LOC)) || (pml == MAX_PAGE_MAP_LEVEL);
}

template<int pml> inline bool PageTableEntry_<pml>::maps_page()
{
	if constexpr (pml == MAX_PAGE_MAP_LEVEL)
		return false;
	else if constexpr (pml == 1)
		return true;
	else
		return !(value & (1 << PTE_PS_BIT_LOC));
}

template<int pml> inline bool PageTableEntry_<pml>::maps_page_table()
{
	if constexpr (pml == MAX_PAGE_MAP_LEVEL)
		return true;
	else if constexpr (pml == 1)
		return false;
	else
		return !!(value & (1 << PTE_PS_BIT_LOC));
}

template<int pml> inline bool PageTableEntry_<pml>::is_global()
{
	return !!(value & (1 << PTE_G_BIT_LOC)) && (pml < MAX_PAGE_MAP_LEVEL);
}

template<int pml> inline bool PageTableEntry_<pml>::is_execute_disabled()
{
	return !!(value & (uint64_t(1) << PTE_XD_BIT_LOC)) && (pml < 3);
}

template<int pml> inline void PageTableEntry_<pml>::set_present(bool present)
{
	value &= uint64_t(present) << PTE_P_BIT_LOC;
}

template<int pml>
inline void PageTableEntry_<pml>::set_write_allowed(bool write_allowed)
{
	if constexpr (pml == 3)
		return;
	value |= uint64_t(write_allowed) << PTE_RW_BIT_LOC;
}

template<int pml>
inline void PageTableEntry_<pml>::set_user_or_supervisor(bool supervisor)
{
	if constexpr (pml == 3)
		return;
	value |= uint64_t(supervisor) << PTE_US_BIT_LOC;
}

template<int pml>
inline void PageTableEntry_<pml>::map_page(PhysAddr page_addr, bool global)
{
	static_assert(pml == 3, "Can't map page at the page map level 3.");

	static constexpr auto PAGE_MASK = PTE_MAKE_PAGE_MASK(CONTROLLED_BITS);
	page_addr &= PAGE_MASK;
	value &= ~PAGE_MASK;
	value |= page_addr;
	value |= int(global) << PTE_G_BIT_LOC;
}

template<int pml>
inline void PageTableEntry_<pml>::map_page_table(PhysAddr pt_addr)
{
	static_assert(pml > 1, "Can't map page table at the page map level 1.");
	pt_addr &= PTE_PT_MASK;
	value &= ~PTE_PT_MASK;
	value |= pt_addr;
}

template<int pml>
inline void PageTableEntry_<pml>::set_execute_disabled(bool execute_disable)
{
	if constexpr (pml == 3)
		return;
	value &= uint64_t(execute_disable) << PTE_XD_BIT_LOC;
}

template<int pml> inline PhysAddr PageTableEntry_<pml>::get_page_table_addr()
{
	static_assert(pml > 1, "Can't get a page table map address from the "
				"page map level 1 table entry.");
	return value & PTE_PT_MASK;
}

template<int pml> inline PhysAddr PageTableEntry_<pml>::get_page_addr()
{
	static_assert(pml == MAX_PAGE_MAP_LEVEL,
		"Can't get a page map address from the page directory pointer "
		"table entry.");
	return value & PTE_MAKE_PAGE_MASK(CONTROLLED_BITS);
}

template<>
inline const unsigned PageTableEntry_<3>::INDEX_BITS = PDP_ENTRY_INDEX_BITS;
template<int pml>
inline const unsigned PageTableEntry_<pml>::INDEX_BITS = PAGE_ENTRY_INDEX_BITS;

template<> inline const unsigned PageTableEntry_<1>::CONTROLLED_BITS = 12;
template<> inline const unsigned PageTableEntry_<2>::CONTROLLED_BITS = 21;
template<> inline const unsigned PageTableEntry_<3>::CONTROLLED_BITS = 30;


struct LinearAddress_within_4KbPage {
	uint32_t offset : 12;
	uint32_t page : PAGE_ENTRY_INDEX_BITS;
	uint32_t page_directory : PAGE_ENTRY_INDEX_BITS;
	uint32_t page_directory_pointer : PDP_ENTRY_INDEX_BITS;
};

struct LinearAddress_within_2MbPage {
	uint32_t offset : 21;
	uint32_t page_directory : PAGE_ENTRY_INDEX_BITS;
	uint32_t page_directory_pointer : PDP_ENTRY_INDEX_BITS;
};

}

#endif
