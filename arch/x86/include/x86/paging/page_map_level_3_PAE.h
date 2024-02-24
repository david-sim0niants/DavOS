#ifndef _x86_PAGING__PAGE_MAP_LEVEL_3_PAE_H__
#define _x86_PAGING__PAGE_MAP_LEVEL_3_PAE_H__

#include <stdint.h>
#include "page_table_entry.h"

namespace x86 {

namespace constants {

constexpr int page_entry_index_bits = 9;
constexpr int pdp_entry_index_bits = 2;

constexpr auto pte_p_bit_loc = 0;
constexpr auto pte_rw_bit_loc = 1;
constexpr auto pte_us_bit_loc = 2;
constexpr auto pte_ps_bit_loc = 7;
constexpr auto pte_g_bit_loc = 8;
constexpr auto pte_make_page_mask(auto PAGE_SHIFT)
{
	return ((PhysAddr(1) << 52) - (1 << PAGE_SHIFT));
}
constexpr auto pte_pt_mask = pte_make_page_mask(12);
constexpr auto pte_xd_bit_loc = 63;

constexpr auto max_possible_phyaddr_bits = 52;

constexpr auto max_pml_having_ps_bit = 2;
constexpr auto max_pml_having_global_bit = 2;
constexpr auto max_pml_having_xd_bit = 2;

}

template<int pml> inline bool PageTableEntry_<pml>::is_present() const
{
	return !!(value & (1 << constants::pte_p_bit_loc));
}

template<int pml> inline bool PageTableEntry_<pml>::is_write_allowed() const
{
	return !!(value & (1 << constants::pte_rw_bit_loc))
		|| (pml == max_page_map_level);
}

template<int pml> inline bool PageTableEntry_<pml>::is_supervisor() const
{
	return !(value & (1 << constants::pte_us_bit_loc))
		|| (pml == max_page_map_level);
}

template<int pml> inline bool PageTableEntry_<pml>::maps_page() const
{
	if constexpr (pml == max_page_map_level)
		return false;
	else if constexpr (pml == 1)
		return true;
	else
		return !!(value & (1 << constants::pte_ps_bit_loc));
}

template<int pml> inline bool PageTableEntry_<pml>::maps_page_table() const
{
	if constexpr (pml == max_page_map_level)
		return true;
	else if constexpr (pml == 1)
		return false;
	else
		return !(value & (1 << constants::pte_ps_bit_loc));
}

template<int pml> inline bool PageTableEntry_<pml>::is_global() const
{
	if constexpr (pml > constants::max_pml_having_global_bit)
		return false;
	else
		return !!(value & (1 << constants::pte_g_bit_loc));
}

template<int pml> inline bool PageTableEntry_<pml>::is_execute_disabled() const
{
	if constexpr (pml > constants::max_pml_having_xd_bit)
		return false;
	else
		return !!(value & (PageTableEntryValue(1) << constants::pte_xd_bit_loc));
}

template<int pml> inline void PageTableEntry_<pml>::set_present(bool present)
{
	static constexpr auto mask = (PageTableEntryValue)1 << constants::pte_p_bit_loc;
	value = (value & ~mask) | (mask * present);
}

template<int pml>
inline void PageTableEntry_<pml>::set_write_allowed(bool write_allowed)
{
	if constexpr (pml == 3)
		return;
	static constexpr auto mask = (PageTableEntryValue)1 << constants::pte_rw_bit_loc;
	value = (value & ~mask) | (mask * write_allowed);
}

template<int pml>
inline void PageTableEntry_<pml>::set_supervisor(bool supervisor)
{
	if constexpr (pml == 3)
		return;
	static constexpr auto mask = (PageTableEntryValue)1 << constants::pte_us_bit_loc;
	value = (value & ~mask) | (mask * !supervisor);
}

template<int pml>
inline void PageTableEntry_<pml>::map_page(PhysAddr page_addr, bool global)
{
	static_assert(pml == 3, "Can't map page at the page map level 3.");

	static constexpr auto page_mask =
		constants::pte_make_page_mask(controlled_bits);
	page_addr &= page_mask;
	value &= ~page_mask;
	value |= page_addr;

	if constexpr (pml <= constants::max_pml_having_global_bit)
		value |= int(global) << constants::pte_g_bit_loc;

	if constexpr (1 < pml && pml <= constants::max_pml_having_ps_bit)
		value |= 1 << constants::pte_ps_bit_loc;
}

template<int pml>
inline void PageTableEntry_<pml>::map_page_table(PhysAddr pt_addr)
{
	static_assert(pml > 1, "Can't map page table at the page map level 1.");
	pt_addr &= constants::pte_pt_mask;
	value &= ~constants::pte_pt_mask;
	value |= pt_addr;

	if constexpr (1 < pml && pml <= constants::max_pml_having_ps_bit)
		value &= ~(1 << constants::pte_ps_bit_loc);
}

template<int pml>
inline void PageTableEntry_<pml>::set_execute_disabled(bool execute_disable)
{
	if constexpr (pml > constants::max_pml_having_xd_bit)
		return;
	static constexpr auto mask = (PageTableEntryValue)1 << constants::pte_xd_bit_loc;
	value = (value & ~mask) | (mask * execute_disable);
}

template<int pml> inline PhysAddr PageTableEntry_<pml>::get_page_table_addr() const
{
	static_assert(pml > 1, "Can't get a page table map address from the "
				"page map level 1 table entry.");
	return value & constants::pte_pt_mask;
}

template<int pml> inline PhysAddr PageTableEntry_<pml>::get_page_addr() const
{
	static_assert(pml == max_page_map_level,
		"Can't get a page map address from the page directory pointer "
		"table entry.");
	return value & constants::pte_make_page_mask(controlled_bits);
}

template<>
inline const unsigned PageTableEntry_<3>::index_bits =
	constants::pdp_entry_index_bits;
template<int pml>
inline const unsigned PageTableEntry_<pml>::index_bits = constants::page_entry_index_bits;

template<>
inline const unsigned PageTableEntry_<1>::controlled_bits = 12;
template<int pml>
inline const unsigned PageTableEntry_<pml>::
	controlled_bits = PageTableEntry_<pml - 1>::controlled_bits
			+ PageTableEntry_<pml - 1>::index_bits;
}

#endif
