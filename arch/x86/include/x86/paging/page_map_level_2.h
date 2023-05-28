#ifndef x86_PAGING__PAGE_MAP_LEVEL_2_H__
#define x86_PAGING__PAGE_MAP_LEVEL_2_H__

#include <stdint.h>
#include <x86/paging/page_table_entry.h>

namespace x86 {

constexpr int PAGE_ENTRY_INDEX_BITS = 10;

constexpr auto PTE_P_BIT_LOC = 0;
constexpr auto PTE_RW_BIT_LOC = 1;
constexpr auto PTE_US_BIT_LOC = 2;
constexpr auto PTE_PS_BIT_LOC = 7;
constexpr auto PTE_G_BIT_LOC = 8;
constexpr auto PTE_MAKE_PAGE_MASK(auto PAGE_SHIFT)
{
	return -(uint32_t(1) << PAGE_SHIFT);
}
constexpr auto PTE_PT_MASK = PTE_MAKE_PAGE_MASK(12);
constexpr auto PTE_XD_BIT_LOC = 63;

template<int pml> inline bool PageTableEntry<pml>::is_present()
{
	return !!(value & (1 << PTE_P_BIT_LOC));
}

template<int pml> inline bool PageTableEntry<pml>::is_write_allowed()
{
	return !!(value & (1 << PTE_RW_BIT_LOC));
}

template<int pml> inline bool PageTableEntry<pml>::is_supervisor()
{
	return !!(value & (1 << PTE_US_BIT_LOC));
}

template<int pml> inline bool PageTableEntry<pml>::maps_page()
{
	return !(value & (1 << PTE_PS_BIT_LOC)) || (pml == 1);
}

template<int pml> inline bool PageTableEntry<pml>::maps_page_table()
{
	return !!(value & (1 << PTE_PS_BIT_LOC)) && (pml != 1);
}

template<int pml> inline bool PageTableEntry<pml>::is_global()
{
	return !!(value & (1 << PTE_G_BIT_LOC));
}

template<int pml> inline bool PageTableEntry<pml>::is_execute_disabled()
{
	return false;
}

template<int pml> inline void PageTableEntry<pml>::set_present(bool present)
{
	value &= uint64_t(present) << PTE_P_BIT_LOC;
}

template<int pml>
inline void PageTableEntry<pml>::set_write_allowed(bool write_allowed)
{
	value |= uint64_t(write_allowed) << PTE_RW_BIT_LOC;
}

template<int pml>
inline void PageTableEntry<pml>::set_user_or_supervisor(bool supervisor)
{
	value |= uint64_t(supervisor) << PTE_US_BIT_LOC;
}

template<int pml>
inline void PageTableEntry<pml>::map_page(PhysAddr page_addr, bool global)
{
	static constexpr auto PAGE_MASK = PTE_MAKE_PAGE_MASK(CONTROLLED_BITS);
	page_addr &= PAGE_MASK;
	value &= ~PAGE_MASK;
	value |= page_addr;
	value |= int(global) << PTE_G_BIT_LOC;
}

template<int pml>
inline void PageTableEntry<pml>::map_page_table(PhysAddr pt_addr)
{
	static_assert(pml > 1, "Can't map page table at the page map level 1.");
	pt_addr &= PTE_PT_MASK;
	value &= ~PTE_PT_MASK;
	value |= pt_addr;
}

template<int pml>
inline void PageTableEntry<pml>::set_execute_disabled(bool execute_disable)
{
}

template<int pml> inline PhysAddr PageTableEntry<pml>::get_page_table_addr()
{
	static_assert(pml > 1,
		"Can't get a page table address from the level 1 entry.");
	return value & PTE_PT_MASK;
}

template<int pml> inline PhysPageN PageTableEntry<pml>::get_page_addr()
{
	return value & PTE_MAKE_PAGE_MASK(CONTROLLED_BITS);
}

template<int pml>
const unsigned PageTableEntry<pml>::INDEX_BITS = PAGE_ENTRY_INDEX_BITS;

template<> inline const unsigned PageTableEntry<1>::CONTROLLED_BITS = 12;
template<> inline const unsigned PageTableEntry<2>::CONTROLLED_BITS = 22;


struct LinearAddress_within_4KbPage {
	uint32_t offset : 12;
	uint32_t page : PAGE_ENTRY_INDEX_BITS;
	uint32_t page_directory : PAGE_ENTRY_INDEX_BITS;
};

struct LinearAddress_within_4MbPage {
	uint32_t offset : 22;
	uint32_t page_directory : PAGE_ENTRY_INDEX_BITS;
};

}

#endif
