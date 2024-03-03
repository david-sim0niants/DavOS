#ifndef _x86_PAGING__PAGE_MAP_H__
#define _x86_PAGING__PAGE_MAP_H__


#include <x86/config.h>
#include <x86/addressing.h>
#include <stddef.h>
#include <kstd/either.h>
#include <kstd/enum.h>
#include <kstd/memory.h>
#include <kstd/maybe.h>


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

/* Get index of entry in a page table with given page map level
 * that controls the memory region the linaddr_beg belongs to.
 * If pml == 0 is given, just return the offset of linaddr_beg within its page. */
template<int pml> inline unsigned get_pte_idx(LineAddr linaddr_beg)
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
			return LineAddr(1) << PageTableEntry_<pml + 1>::controlled_bits;
	}();

	return ((end_bit - beg_bit) & linaddr_beg) >> beg_bit_loc;
}

/* Enum denoting a page size. */
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

/* Page size enums in order. */
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

/* Array of page size shifts in order. */
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

/* Get page size shift from page size enum. */
inline constexpr auto get_page_size_shift(PageSize page_size)
{
	return page_size_shifts[(unsigned)page_size];
}

/* Get page size in bytes from page size enum. */
inline constexpr auto get_page_size_bytes(PageSize page_size)
{
	return 1 << get_page_size_shift(page_size);
}


/* Page mapping errors. */
enum class PageMapErr {
	None = 0, // No error.
	ExistingPageMap, // A page is already mapped.
	NoFreeMem, // No free memory left in the free memory region.
	UnalignedAddress, // Unaligned linear or physical addresses.
	AddressMismatch, // Linear and physical addresses don't have a common alignment to the given page size.
	LinearAddressOverflow, // self-explanatory
	PhysicalAddressOverflow, // self-explanatory
};

/* Struct containing how page mapping should be done. */
struct PageMappingInfo {
	LineAddr linaddr_beg; /* Beginning linear address. */
	PhysAddr phyaddr_beg; /* Beginning physical address. */
	PhysAddr phyaddr_end; /* Ending physical address. */
	PageSize page_size; /* Page size. Specify it only for functions that require it. */
	PageEntryFlags flags; /* Page entry flags. */
};

/* Class representing a page table with given page map level
 * for low level page mapping operations. */
template<int pml>
class PageTable_ {
public:
	friend class PageTable_<pml + 1>;

	/* Map linear address memory
	 * [info.linaddr_beg; info.linaddr_beg + (info.phyaddr_end - info.phyaddr_beg)) to
	 * physical address memory   [info.phyaddr_beg, info.phyaddr_end)
	 * given the free physical memory [free_mem.beg, free_mem.end)
	 * where page tables can be stored.
	 * Mapped pages may have different sizes if necessary.
	 * In case of any error, especially PageMapErr::NoFreeMem, the info struct
	 * will be updated to contain the linear and physical addresses at which
	 * mapping failed and the free_mem will contain the rest of the free memory
	 * used for mapping. */
	PageMapErr map_memory(PageMappingInfo& info, kstd::MemoryRange& free_mem);

	/* Map pages the size of page_size of linear address memory
	 * [info.linaddr_beg; info.linaddr_beg + (info.phyaddr_end - info.phyaddr_beg)) to
	 * physical address memory   [info.phyaddr_beg, info.phyaddr_end)
	 * given the free physical memory [free_mem.beg, free_mem.end)
	 * where page tables can be stored.
	 * Mapped pages may have different sizes if necessary.
	 * In case of any error, especially PageMapErr::NoFreeMem, the info struct
	 * will be updated to contain the linear and physical addresses at which
	 * mapping failed and the free_mem will contain the rest of the free memory
	 * used for mapping. */
	PageMapErr map_pages(PageMappingInfo& info, kstd::MemoryRange& free_mem);

	const PageTableEntry_<pml> *observe() const;

	/* Number of page table entries. */
	static constexpr auto nr_entries= 1 << PageTableEntry_<pml>::index_bits;
	/* Total size of the page table. */
	static constexpr auto size = nr_entries * sizeof(PageTableEntry_<pml>);
	/* Memory controlled by each page table entry. */
	static constexpr auto controlled_mem_per_entry
		= LineSize(1) << PageTableEntry_<pml>::controlled_bits;
	/* Memory controlled by this page table. */
	static constexpr auto controlled_mem = controlled_mem_per_entry * nr_entries;
	/* Shift of page table size. */
	static constexpr unsigned int size_shift = 12;

private:
	/* Same as map_pages__no_mm but the _no_chk suffix means it won't check
	 * linaddr_beg and phyaddr_beg alignments and whether any overflow might happen. */
	PageMapErr map_pages__no_chk(PageMappingInfo& info, kstd::MemoryRange& free_mem);

	/* Same as map_pages__no_mm_no_chk but the page_size is a compile time constant. */
	template<PageSize page_size>
	PageMapErr map_pages__const_ps(PageMappingInfo& info, kstd::MemoryRange& free_mem);

	/* Check if linear and physical addresses will overflow during mapping. */
	static PageMapErr check_overflow(LineAddr linaddr_beg, PhysAddr phyaddr_beg,
			LineAddr linaddr_end);

	/* Get a page table that the given entry links to if it does, otherwise
	 * create, map and get a new page table using the given free memory range. */
	static kstd::Either<PageTable_<pml - 1> *, PageMapErr> get_or_map_page_table(
			PageTableEntry_<pml>& entry, kstd::MemoryRange& free_mem);

	/* The exact array of entries. */
	alignas(sizeof(PageTableEntryValue) * nr_entries)
	PageTableEntry_<pml> entries[nr_entries] = {};
};

template<int pml>
inline const PageTableEntry_<pml> *PageTable_<pml>::observe() const
{
	return entries;
}

constexpr kstd::Maybe<PageSize> find_max_page_size(
		LineAddr linaddr_beg, PhysAddr phyaddr_beg, size_t size_limit)
{
	for (int i = num_page_sizes - 1; i >= 0; --i) {
		PageSize page_size = page_sizes[i];
		auto page_size_bytes = get_page_size_bytes(page_size);

		if ( 	(page_size_bytes > size_limit) 		||
			(linaddr_beg & (page_size_bytes - 1)) 	||
			(phyaddr_beg & (page_size_bytes - 1))
		)
			continue;
		return page_size;
	}

	return {};
}

/* Page table with highest page map level able to control whole linear memory. */
using PageTable = PageTable_<max_page_map_level>;
using PageTableEntry = PageTableEntry_<max_page_map_level>;

static constexpr LineAddr page_fit_linear_addr(LineAddr addr)
{
	return addr & (PageTable::controlled_mem - 1);
}

}

#endif
