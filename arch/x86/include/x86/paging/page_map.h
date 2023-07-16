#ifndef _x86_PAGING__PAGE_MAP_H__
#define _x86_PAGING__PAGE_MAP_H__


#include <x86/config.h>
#include <x86/addressing.h>
#include <stddef.h>
#include <kstd/either.h>
#include <kstd/enum.h>


#if CONFIG_ARCH == ARCH_x86_64
	#if CONFIG_x86_PAGE_MAP_LEVEL == x86_PAGE_MAP_LEVEL_4 || \
		CONFIG_x86_PAGE_MAP_LEVEL == x86_PAGE_MAP_LEVEL_5
		#include "page_map_level_4_5.h"
	#else
		#error "At least one kind of paging map level should be specified for x86_64 architecture."
	#endif
#else
	#if (CONFIG_x86_PAGE_MAP_LEVEL == x86_PAGE_MAP_LEVEL_2) || \
		(CONFIG_x86_PAGE_MAP_LEVEL == x86_PAGE_MAP_LEVEL_3_PAE)
		#if CONFIG_x86_PAGE_MAP_LEVEL == x86_PAGE_MAP_LEVEL_2
			#include "page_map_level_2.h"
		#endif
		#if CONFIG_x86_PAGE_MAP_LEVEL == x86_PAGE_MAP_LEVEL_3_PAE
			#include "page_map_level_3_PAE.h"
		#endif
	#else
		#error "At least one kind of paging map level should be specified for i386 architecture."
	#endif
#endif

namespace x86 {

enum class PageEntryFlags {
	None = 0x0,
	WriteAllowed = 0x1,
	Supervisor = WriteAllowed << 1,
	Global = Supervisor << 1,
	ExecuteDisabled = Global << 1,
};
KSTD_DEFINE_ENUM_LOGIC_BITWISE_OPERATORS(PageEntryFlags);

template<int pml> inline unsigned get_pte_idx(LineAddr linaddr)
{
	static_assert(pml >= 0 && pml <= max_page_map_level,
		"Invalid page map level.");

	constexpr auto beg_bit_loc = []() constexpr
	{
		if constexpr (pml == 0)
			return 0;
		else
			return PageTableEntry_<pml>::controlled_bits;
	}();
	constexpr auto beg_bit = LineAddr(1) << beg_bit_loc;

	constexpr auto end_bit = []() constexpr -> LineAddr
	{
		if constexpr (pml == max_page_map_level)
			return 0;
		else
			return LineAddr(1)
				<< PageTableEntry_<pml + 1>::controlled_bits;
	}();

	return ((end_bit - beg_bit) & linaddr) >> beg_bit_loc;
}

enum class PageSize {
	_4Kb = 0,
#if CONFIG_x86_PAGE_MAP_LEVEL >= x86_PAGE_MAP_LEVEL_3_PAE
	_2Mb,
#endif
#if CONFIG_x86_PAGE_MAP_LEVEL == x86_PAGE_MAP_LEVEL_2
	_4Mb,
#endif
#if CONFIG_x86_PAGE_MAP_LEVEL >= x86_PAGE_MAP_LEVEL_4
	_1Gb,
#endif
};

constexpr PageSize page_sizes[] = {
	PageSize::_4Kb,
#if CONFIG_x86_PAGE_MAP_LEVEL >= x86_PAGE_MAP_LEVEL_3_PAE
	PageSize::_2Mb,
#endif
#if CONFIG_x86_PAGE_MAP_LEVEL == x86_PAGE_MAP_LEVEL_2
	PageSize::_4Mb,
#endif
#if CONFIG_x86_PAGE_MAP_LEVEL >= x86_PAGE_MAP_LEVEL_4
	PageSize::_1Gb,
#endif
};

constexpr auto num_page_sizes = sizeof(page_sizes) / sizeof(page_sizes[0]);

constexpr unsigned page_size_shifts[] = {
	12,
#if CONFIG_x86_PAGE_MAP_LEVEL >= x86_PAGE_MAP_LEVEL_3_PAE
	21,
#endif
#if CONFIG_x86_PAGE_MAP_LEVEL == x86_PAGE_MAP_LEVEL_2
	22,
#endif
#if CONFIG_x86_PAGE_MAP_LEVEL >= x86_PAGE_MAP_LEVEL_4
	30,
#endif
};

inline constexpr auto get_page_size_shift(PageSize page_size)
{
	return page_size_shifts[(unsigned)page_size];
}

inline constexpr auto get_page_size_bytes(PageSize page_size)
{
	return 1 << get_page_size_shift(page_size);
}


enum class PageMapErr {
	None = 0,
	ExistingPageMap,
	MapUserOnSupervisorArea,
	MapWriteAllowedOnReadOnlyArea,
	MapExecutableOnXDArea,
	NoFreeMem,
	UnalignedAddress,
	AddressMismatch,
	LinearAddressOverflow,
	PhysicalAddressOverflow,
};

template<int pml>
class PageTable_ {
public:
	friend class PageTable_<pml + 1>;

	PageMapErr map_memory__no_mm(LineAddr linaddr, PhysAddr phyaddr,
			size_t mem_size, PageEntryFlags flags,
			uintptr_t& free_mem_beg, uintptr_t free_mem_end);

	PageMapErr map_pages__no_mm(LineAddr linaddr, PhysAddr phyaddr,
			LinePageN nr_pages, PageSize page_size,
			PageEntryFlags flags,
			uintptr_t& free_mem_beg, uintptr_t free_mem_end);

	static constexpr auto nr_entries= 1 << PageTableEntry_<pml>::index_bits;
	static constexpr auto size = nr_entries * sizeof(PageTableEntry_<pml>);
	static constexpr auto controlled_mem_per_entry
		= LineSize(1) << PageTableEntry_<pml>::controlled_bits;
	static constexpr auto
		controlled_mem = controlled_mem_per_entry * nr_entries;

private:
	PageMapErr map_pages__no_mm_no_chk(LineAddr linaddr, PhysAddr phyaddr,
			LinePageN nr_pages, PageSize page_size,
			PageEntryFlags flags,
			uintptr_t& free_mem_beg, uintptr_t free_mem_end);

	template<PageSize page_size>
	PageMapErr map_pages__no_mm_const_ps(LineAddr linaddr, PhysAddr phyaddr,
			LinePageN nr_pages, PageEntryFlags flags,
			uintptr_t& free_mem_beg, uintptr_t free_mem_end);

	static PageMapErr check_overflow(LineAddr linaddr, PhysAddr phyaddr,
			size_t mem_size);

	static
	kstd::Either<PageTable_<pml - 1> *, PageMapErr> get_or_map_page_table(
			PageTableEntry_<pml>& entry,
			uintptr_t& free_mem_beg, uintptr_t free_mem_end);

public:
	PageTableEntry_<pml> entries[nr_entries] = {};
};

using PageTable = PageTable_<max_page_map_level>;
using PageTableEntry = PageTableEntry_<max_page_map_level>;

}

#endif
