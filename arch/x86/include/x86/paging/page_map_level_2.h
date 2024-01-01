#ifndef _x86_PAGING__PAGE_MAP_LEVEL_2_H__
#define _x86_PAGING__PAGE_MAP_LEVEL_2_H__

#include <stdint.h>
#include "page_table_entry.h"

namespace x86 {

namespace constants {

constexpr int page_entry_index_bits = 10;

constexpr auto pte_p_bit_loc = 0;
constexpr auto pte_rw_bit_loc = 1;
constexpr auto pte_us_bit_loc = 2;
constexpr auto pte_ps_bit_loc = 7;
constexpr auto pte_g_bit_loc = 8;
constexpr auto pte_pt_mask = -(uint32_t(1) << 12);
constexpr auto pte_page_mask = -(uint32_t(1) << 12);
constexpr auto pte_l2_page_mask_low = -(uint32_t(1) << 22);
constexpr auto pte_l2_page_mask_high = (uint32_t(1) << 21) - (1 << 13);

constexpr auto max_possible_phyaddr_bits = 40;

}

template<int pml> inline bool PageTableEntry_<pml>::is_present() const
{
	return !!(value & (1 << constants::pte_p_bit_loc));
}

template<int pml> inline bool PageTableEntry_<pml>::is_write_allowed() const
{
	return !!(value & (1 << constants::pte_rw_bit_loc));
}

template<int pml> inline bool PageTableEntry_<pml>::is_supervisor() const
{
	return !!(value & (1 << constants::pte_us_bit_loc));
}

template<int pml> inline bool PageTableEntry_<pml>::maps_page() const
{
	return !!(value & (1 << constants::pte_ps_bit_loc)) || (pml == 1);
}

template<int pml> inline bool PageTableEntry_<pml>::maps_page_table() const
{
	return !(value & (1 << constants::pte_ps_bit_loc)) && (pml != 1);
}

template<int pml> inline bool PageTableEntry_<pml>::is_global() const
{
	return !!(value & (1 << constants::pte_g_bit_loc));
}

template<int pml> inline bool PageTableEntry_<pml>::is_execute_disabled() const
{
	return false;
}

template<int pml> inline void PageTableEntry_<pml>::set_present(bool present)
{
	value &= uint64_t(present) << constants::pte_p_bit_loc;
}

template<int pml>
inline void PageTableEntry_<pml>::set_write_allowed(bool write_allowed)
{
	value |= uint64_t(write_allowed) << constants::pte_rw_bit_loc;
}

template<int pml>
inline void PageTableEntry_<pml>::set_user_or_supervisor(bool supervisor)
{
	value |= uint64_t(supervisor) << constants::pte_us_bit_loc;
}

template<int pml>
inline void PageTableEntry_<pml>::map_page(PhysAddr page_addr, bool global)
{
	uint32_t page_addr_fields;
	if constexpr (pml == 1) {
		page_addr_fields = page_addr & constants::pte_page_mask;
	} else {
		page_addr_fields = page_addr & constants::pte_l2_page_mask_low;
		// 19 is the amount of bits you need to shift to make the
		// 32th bit of the physical page address appear at the 13th bit
		// of the page entry which corresponds to the least significant
		// bit of the high MAXPHYADDR:32 bits of the physical address.
		page_addr_fields |=
			(page_addr >> 19) & constants::pte_l2_page_mask_high;
	}
	value &= ~constants::pte_page_mask;
	value |= page_addr_fields;
	value |= int(global) << constants::pte_g_bit_loc;

	if (pml > 1)
		value |= 1 << constants::pte_ps_bit_loc;
}

template<int pml>
inline void PageTableEntry_<pml>::map_page_table(PhysAddr pt_addr)
{
	static_assert(pml > 1, "Can't map page table at the page map level 1.");
	pt_addr &= constants::pte_pt_mask;
	value &= ~constants::pte_pt_mask;
	value |= pt_addr;

	if (pml > 1)
		value &= ~(1 << constants::pte_ps_bit_loc);
}

template<int pml>
inline void PageTableEntry_<pml>::set_execute_disabled(bool execute_disable)
{
}

template<int pml> inline PhysAddr PageTableEntry_<pml>::get_page_table_addr() const
{
	static_assert(pml > 1,
		"Can't get a page table address from the level 1 entry.");
	return PhysAddr(value) & constants::pte_pt_mask;
}

template<int pml> inline PhysAddr PageTableEntry_<pml>::get_page_addr() const
{
	if constexpr (pml == 1)
		return PhysAddr(value) & constants::pte_page_mask;
	else
		return (PhysAddr(value) & constants::pte_l2_page_mask_low)
			|
		((PhysAddr(value) & constants::pte_l2_page_mask_high) << 19);
}

template<int pml>
const unsigned PageTableEntry_<pml>::index_bits =
	constants::page_entry_index_bits;

template<> inline const unsigned PageTableEntry_<1>::controlled_bits = 12;
template<> inline const unsigned PageTableEntry_<2>::controlled_bits = 22;

}

#endif
