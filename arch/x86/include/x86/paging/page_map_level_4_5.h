#ifndef _x86_PAGING__PAGE_MAP_LEVEL_4_5_H__
#define _x86_PAGING__PAGE_MAP_LEVEL_4_5_H__

#include <stdint.h>
#include "page_table_entry.h"

namespace x86 {

namespace constants {

constexpr int page_entry_index_bits = 9;

constexpr auto pte_p_bit_loc = 0;
constexpr auto pte_rw_bit_loc = 1;
constexpr auto pte_us_bit_loc = 2;
constexpr auto pte_ps_bit_loc = 7;
constexpr auto pte_g_bit_loc = 8;
constexpr auto pte_make_page_mask(auto page_shift)
{
	return ((PhysAddr(1) << 52) - (1 << page_shift));
}
constexpr auto pte_pt_mask = pte_make_page_mask(12);
constexpr auto pte_xd_bit_loc = 63;

constexpr auto max_possible_phyaddr_bits = 52;

constexpr auto max_pml_having_ps_bit = 3;
constexpr auto max_pml_having_global_bit = 3;

}

template<int pml> inline bool PageTableEntry_<pml>::is_present()
{
	return !!(value & (1 << constants::pte_p_bit_loc));
}

template<int pml> inline bool PageTableEntry_<pml>::is_write_allowed()
{
	return !!(value & (1 << constants::pte_rw_bit_loc));
}

template<int pml> inline bool PageTableEntry_<pml>::is_supervisor()
{
	return !!(value & (1 << constants::pte_us_bit_loc));
}

template<int pml> inline bool PageTableEntry_<pml>::maps_page()
{
	if constexpr (pml > constants::max_pml_having_ps_bit)
		return false;
	else if constexpr (pml == 1)
		return true;
	else
		return !!(value & (1 << constants::pte_ps_bit_loc));
}

template<int pml> inline bool PageTableEntry_<pml>::maps_page_table()
{
	if constexpr (pml > constants::max_pml_having_ps_bit)
		return true;
	else if constexpr (pml == 1)
		return false;
	else
		return !(value & (1 << constants::pte_ps_bit_loc));
}

template<int pml> inline bool PageTableEntry_<pml>::is_global()
{
	if constexpr (pml > constants::max_pml_having_global_bit)
		return false;
	else
		return !!(value & (1 << constants::pte_g_bit_loc));
}

template<int pml> inline bool PageTableEntry_<pml>::is_execute_disabled()
{
	return !!(value & (uint64_t(1) << constants::pte_xd_bit_loc));
}

template<int pml> inline void PageTableEntry_<pml>::set_present(bool present)
{
	value |= uint64_t(present) << constants::pte_p_bit_loc;
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
	static_assert(pml <= 3, "Can't map page at the page map level above 3.");

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
	value |= uint64_t(execute_disable) << constants::pte_xd_bit_loc;
}

template<int pml> inline PhysAddr PageTableEntry_<pml>::get_page_addr()
{
	static_assert(pml <= 3,
		"Can't get a page map address at the page map level above 3.");
	return value & constants::pte_make_page_mask(controlled_bits);
}

template<int pml> inline PhysAddr PageTableEntry_<pml>::get_page_table_addr()
{
	static_assert(pml > 1,
		"Can't get a page table address from the level 1 entry.");
	return value & constants::pte_pt_mask;
}

template<int pml>
const unsigned PageTableEntry_<pml>::index_bits =
	constants::page_entry_index_bits;

template<>
inline const unsigned PageTableEntry_<1>::controlled_bits = 12;
template<int pml>
inline const unsigned PageTableEntry_<pml>::
	controlled_bits = PageTableEntry_<pml - 1>::controlled_bits
			+ PageTableEntry_<pml - 1>::index_bits;

}

#endif
