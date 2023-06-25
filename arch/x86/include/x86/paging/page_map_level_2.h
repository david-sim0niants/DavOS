#ifndef _x86_PAGING__PAGE_MAP_LEVEL_2_H__
#define _x86_PAGING__PAGE_MAP_LEVEL_2_H__

#include <stdint.h>
#include "page_table_entry.h"

namespace x86 {

constexpr int PAGE_ENTRY_INDEX_BITS = 10;

constexpr auto PTE_P_BIT_LOC = 0;
constexpr auto PTE_RW_BIT_LOC = 1;
constexpr auto PTE_US_BIT_LOC = 2;
constexpr auto PTE_PS_BIT_LOC = 7;
constexpr auto PTE_G_BIT_LOC = 8;
constexpr auto PTE_PT_MASK = -(uint32_t(1) << 12);
constexpr auto PTE_PAGE_MASK = -(uint32_t(1) << 12);
constexpr auto PTE_L2_PAGE_MASK_LOW = -(uint32_t(1) << 22);
constexpr auto PTE_L2_PAGE_MASK_HIGH = (uint32_t(1) << 21) - (1 << 13);
constexpr auto PTE_XD_BIT_LOC = 63;

static constexpr auto MAX_POSSIBLE_PHYADDR_BITS = 40;

template<int pml> inline bool PageTableEntry_<pml>::is_present()
{
	return !!(value & (1 << PTE_P_BIT_LOC));
}

template<int pml> inline bool PageTableEntry_<pml>::is_write_allowed()
{
	return !!(value & (1 << PTE_RW_BIT_LOC));
}

template<int pml> inline bool PageTableEntry_<pml>::is_supervisor()
{
	return !!(value & (1 << PTE_US_BIT_LOC));
}

template<int pml> inline bool PageTableEntry_<pml>::maps_page()
{
	return !!(value & (1 << PTE_PS_BIT_LOC)) || (pml == 1);
}

template<int pml> inline bool PageTableEntry_<pml>::maps_page_table()
{
	return !(value & (1 << PTE_PS_BIT_LOC)) && (pml != 1);
}

template<int pml> inline bool PageTableEntry_<pml>::is_global()
{
	return !!(value & (1 << PTE_G_BIT_LOC));
}

template<int pml> inline bool PageTableEntry_<pml>::is_execute_disabled()
{
	return false;
}

template<int pml> inline void PageTableEntry_<pml>::set_present(bool present)
{
	value &= uint64_t(present) << PTE_P_BIT_LOC;
}

template<int pml>
inline void PageTableEntry_<pml>::set_write_allowed(bool write_allowed)
{
	value |= uint64_t(write_allowed) << PTE_RW_BIT_LOC;
}

template<int pml>
inline void PageTableEntry_<pml>::set_user_or_supervisor(bool supervisor)
{
	value |= uint64_t(supervisor) << PTE_US_BIT_LOC;
}

template<int pml>
inline void PageTableEntry_<pml>::map_page(PhysAddr page_addr, bool global)
{
	uint32_t page_addr_fields;
	if constexpr (pml == 1) {
		page_addr_fields = page_addr & PTE_PAGE_MASK;
	} else {
		page_addr_fields = page_addr & PTE_L2_PAGE_MASK_LOW;
		// 19 is the amount of bits you need to shift to make the
		// 32th bit of the physical page address appear at the 13th bit
		// of the page entry which corresponds to the least significant
		// bit of the high MAXPHYADDR:32 bits of the physical address.
		page_addr_fields |= (page_addr >> 19) & PTE_L2_PAGE_MASK_HIGH;
	}
	value &= ~PTE_PAGE_MASK;
	value |= page_addr_fields;
	value |= int(global) << PTE_G_BIT_LOC;

	if (pml > 1)
		value |= 1 << PTE_PS_BIT_LOC;
}

template<int pml>
inline void PageTableEntry_<pml>::map_page_table(PhysAddr pt_addr)
{
	static_assert(pml > 1, "Can't map page table at the page map level 1.");
	pt_addr &= PTE_PT_MASK;
	value &= ~PTE_PT_MASK;
	value |= pt_addr;

	if (pml > 1)
		value &= ~(1 << PTE_PS_BIT_LOC);
}

template<int pml>
inline void PageTableEntry_<pml>::set_execute_disabled(bool execute_disable)
{
}

template<int pml> inline PhysAddr PageTableEntry_<pml>::get_page_table_addr()
{
	static_assert(pml > 1,
		"Can't get a page table address from the level 1 entry.");
	return PhysAddr(value) & PTE_PT_MASK;
}

template<int pml> inline PhysAddr PageTableEntry_<pml>::get_page_addr()
{
	if constexpr (pml == 1)
		return PhysAddr(value) & PTE_PAGE_MASK;
	else
		return (PhysAddr(value) & PTE_L2_PAGE_MASK_LOW)
			| ((PhysAddr(value) & PTE_L2_PAGE_MASK_HIGH) << 19);
}

template<int pml>
const unsigned PageTableEntry_<pml>::INDEX_BITS = PAGE_ENTRY_INDEX_BITS;

template<> inline const unsigned PageTableEntry_<1>::CONTROLLED_BITS = 12;
template<> inline const unsigned PageTableEntry_<2>::CONTROLLED_BITS = 22;

}

#endif
