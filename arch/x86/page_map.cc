#include <x86/paging/page_map.h>

#include <string.h>
#include <compiler_attributes.h>

#include <kstd/overflow.h>
#include <kstd/either.h>
#include <kstd/maybe.h>
#include <kstd/algorithm.h>

namespace x86 {

enum class LocalErr {
	None = 0,
	Overflow,
	NoFreeMemory,
};

static LocalErr align_address_to_floor(unsigned long& addr, size_t alignment);

template<int pml> static kstd::Either<PhysAddr, LocalErr> create_page_table(
		uintptr_t& free_mem_beg, uintptr_t free_mem_end);

template<int pml>
static void set_entry_flags(PageTableEntry_<pml>& entry, PageEntryFlags flags);

static bool check_alignment(LineAddr linaddr, PhysAddr phyaddr, PageSize ps);

static kstd::Maybe<PageSize> find_min_page_size(
		LineAddr linaddr, PhysAddr phyaddr, size_t size_limit);


template<int pml> PageMapErr PageTable_<pml>::map_memory__no_mm(
		LineAddr linaddr, PhysAddr phyaddr, size_t mem_size,
		PageEntryFlags flags,
		uintptr_t& free_mem_beg, uintptr_t free_mem_end)
{
	auto e = check_overflow(linaddr, phyaddr, mem_size);
	if (e != PageMapErr::None)
		return e;

	while (mem_size > 0) {
		const auto maybe_page_size =
			find_min_page_size(linaddr, phyaddr, mem_size);
		if (!maybe_page_size.has_value())
			return PageMapErr::AddressMismatch;
		auto page_size = *maybe_page_size;
		auto page_size_bytes = get_page_size_bytes(page_size);
		auto page_size_shift = get_page_size_shift(page_size);

		auto nr_pages = mem_size >> page_size_shift;
		auto e = map_pages__no_mm_no_chk(linaddr, phyaddr, nr_pages,
				page_size, flags, free_mem_beg, free_mem_end);
		if (e != PageMapErr::None)
			return e;
		mem_size -= nr_pages << page_size_shift;
	}

	return PageMapErr::None;
}

template<int pml> PageMapErr PageTable_<pml>::map_pages__no_mm(
		LineAddr linaddr, PhysAddr phyaddr, LinePageN nr_pages,
		PageSize page_size, PageEntryFlags flags,
		uintptr_t& free_mem_beg, uintptr_t free_mem_end)
{
	if (!check_alignment(linaddr, phyaddr, page_size))
		return PageMapErr::UnalignedAddress;

	const auto page_size_shift = get_page_size_shift(page_size);
	const auto mem_size = LineSize(nr_pages) << page_size_shift;

	auto e = check_overflow(linaddr, phyaddr, mem_size);
	if (e != PageMapErr::None)
		return e;

	return map_pages__no_mm_no_chk(linaddr, phyaddr, nr_pages,
			page_size, flags, free_mem_beg, free_mem_end);
}

template<int pml> PageMapErr PageTable_<pml>::map_pages__no_mm_no_chk(
		LineAddr linaddr, PhysAddr phyaddr, LinePageN nr_pages,
		PageSize page_size, PageEntryFlags flags,
		uintptr_t& free_mem_beg, uintptr_t free_mem_end)
{
	switch (page_size) {
	case PageSize::_4Kb:
		return map_pages__no_mm_const_ps<PageSize::_4Kb>(
				linaddr, phyaddr, nr_pages, flags,
				free_mem_beg, free_mem_end);
#if CONFIG_x86_PAGE_MAP_LEVEL >= x86_PAGE_MAP_LEVEL_3_PAE
	case PageSize::_2Mb:
		return map_pages__no_mm_const_ps<PageSize::_2Mb>(
				linaddr, phyaddr, nr_pages, flags,
				free_mem_beg, free_mem_end);
#endif
#if CONFIG_x86_PAGE_MAP_LEVEL == x86_PAGE_MAP_LEVEL_2
	case PageSize::_4Mb:
		return map_pages__no_mm_const_ps<PageSize::_4Mb>(
				linaddr, phyaddr, nr_pages, flags,
				free_mem_beg, free_mem_end);
#endif
#if CONFIG_x86_PAGE_MAP_LEVEL >= x86_PAGE_MAP_LEVEL_4
	case PageSize::_1Gb:
		return map_pages__no_mm_const_ps<PageSize::_1Gb>(
				linaddr, phyaddr, nr_pages, flags,
				free_mem_beg, free_mem_end);
#endif
	};

	return PageMapErr::None;
}

template<int pml> template<PageSize page_size>
__FORCE_INLINE PageMapErr PageTable_<pml>::map_pages__no_mm_const_ps(
		LineAddr linaddr, PhysAddr phyaddr, LinePageN nr_pages,
		PageEntryFlags flags,
		uintptr_t& free_mem_beg, uintptr_t free_mem_end)
{
	using kstd::test;
	static constexpr auto
		controlled_bits = PageTableEntry_<pml>::controlled_bits;

	constexpr auto page_size_shift = get_page_size_shift(page_size);

	static_assert(page_size_shift <= controlled_bits,
		"Page size larger than the controlled memory region at the "
		"current page map level.\n"
		"NOTE: it was probably smaller than the controlled memory "
		"region at the above page map level which means it's an "
		"invalid value.");

	if constexpr (page_size_shift == controlled_bits) {
		auto curr_pn = get_pte_idx<pml>(linaddr);
		auto end_pn = curr_pn + nr_pages;

		constexpr auto page_size_nb = 1 << page_size_shift;

		for (; curr_pn < end_pn; ++curr_pn, phyaddr += page_size_nb) {
			auto& entry = entries[curr_pn];
			if (entry.is_present())
				return PageMapErr::ExistingPageMap;

			entry.map_page(phyaddr,
					test(flags, PageEntryFlags::Global));
			set_entry_flags(entry, flags);
			entry.set_present(true);
		}
	} else {
		static_assert(pml > 1, "Can't have pml == 1 here.");

		static constexpr LineSize
			mem_region_size = LineSize(1) << controlled_bits;
		static constexpr LinePageN
			mem_region_pages = mem_region_size >> page_size_shift;
		static constexpr LinePageN
			mem_region_pages_mask = mem_region_pages - 1;

		auto curr_rn = get_pte_idx<pml>(linaddr);

		for (; nr_pages > 0 ; ++curr_rn) {
			const LinePageN page_num = linaddr >> page_size_shift;
			LinePageN nr_pages_to_map = mem_region_pages -
				(page_num & mem_region_pages_mask);
			nr_pages_to_map = kstd::min(nr_pages_to_map, nr_pages);

			PageTableEntry_<pml>& entry = entries[curr_rn];

			entry.set_write_allowed(
				entry.is_write_allowed() ||
				test(flags, PageEntryFlags::WriteAllowed));
			entry.set_user_or_supervisor(
				entry.is_supervisor() &&
				test(flags, PageEntryFlags::Supervisor));
			entry.set_execute_disabled(
				entry.is_execute_disabled() &&
				test(flags, PageEntryFlags::ExecuteDisabled));

			auto next_pt_or_err = get_or_map_page_table(
					entry, free_mem_beg, free_mem_end);

			if (auto *e = kstd::try_get<PageMapErr>(next_pt_or_err))
				return *e;
			auto *next_pt = kstd::get<PageTable_<pml - 1> *>(
						next_pt_or_err);
			auto e =
			next_pt->template map_pages__no_mm_const_ps<page_size>(
					linaddr, phyaddr, nr_pages_to_map,
					flags, free_mem_beg, free_mem_end);
			if (e != PageMapErr::None)
				return e;

			nr_pages -= nr_pages_to_map;
			linaddr += nr_pages_to_map << page_size_shift;
		}
	}

	return PageMapErr::None;
}

template<int pml>
PageMapErr PageTable_<pml>::check_overflow(LineAddr linaddr, PhysAddr phyaddr,
		size_t mem_size)
{
	LineAddr linaddr_end;
	if (kstd::add_overflow(linaddr, mem_size, linaddr_end)
	 || linaddr_end >= controlled_mem)
		return PageMapErr::LinearAddressOverflow;

	PhysAddr phyaddr_end;
	if (kstd::add_overflow(phyaddr, mem_size, phyaddr_end)
	 || phyaddr_end >= (LineAddr(1)<< constants::max_possible_phyaddr_bits))
		return PageMapErr::PhysicalAddressOverflow;

	return PageMapErr::None;
}

template<int pml> kstd::Either<PageTable_<pml - 1> *, PageMapErr> PageTable_<pml>::get_or_map_page_table(
		PageTableEntry_<pml>& entry,
		uintptr_t& free_mem_beg, uintptr_t free_mem_end)
{
	static_assert(pml > 1, "Can't have pml == 1 here.");
	PhysAddr next_pt_addr;

	if (entry.is_present()) {
		if (entry.maps_page())
			return PageMapErr::ExistingPageMap;
		// if reached here then the entry does map a page table
		next_pt_addr = entry.get_page_table_addr();
	} else {
		auto maybe_pt_ptr = create_page_table<pml - 1>(
			free_mem_beg, free_mem_end);

		auto *p_err = kstd::try_get<LocalErr>(maybe_pt_ptr);
		if (p_err && *p_err == LocalErr::NoFreeMemory)
			return PageMapErr::NoFreeMem;

		next_pt_addr = kstd::get<PhysAddr>(maybe_pt_ptr);
		entry.map_page_table((PhysAddr)next_pt_addr);
		entry.set_present(true);
	}

	return reinterpret_cast<PageTable_<pml - 1> *>(next_pt_addr);
}

template class PageTable_<max_page_map_level>;


static LocalErr align_address_to_floor(unsigned long& addr, size_t alignment)
{
	const auto prev_addr = addr;
	addr = (addr / alignment) * alignment;
	if (addr < prev_addr) [[likely]] {
		if (kstd::add_overflow(addr, alignment)) [[unlikely]]
			return LocalErr::Overflow;
	}
	return LocalErr::None;
}

template<int pml>
static kstd::Either<PhysAddr, LocalErr> create_page_table(
		uintptr_t& free_mem_beg, uintptr_t free_mem_end)
{
	auto constexpr PT_SIZE = PageTable_<pml>::size;
	auto e = align_address_to_floor(free_mem_beg, PT_SIZE);
	if (e == LocalErr::Overflow) [[unlikely]]
		return LocalErr::NoFreeMemory;

	auto new_pt_ptr = free_mem_beg;
	if (kstd::add_overflow(free_mem_beg, PT_SIZE)) [[unlikely]]
		return LocalErr::NoFreeMemory;

	if (free_mem_beg > free_mem_end)
		return LocalErr::NoFreeMemory;

	memset((void *)new_pt_ptr, 0, PageTable_<pml>::size);
	return (PhysAddr)new_pt_ptr;
}

template<int pml>
static void set_entry_flags(PageTableEntry_<pml>& entry, PageEntryFlags flags)
{
	using kstd::test;
	entry.set_write_allowed(test(flags,PageEntryFlags::WriteAllowed));
	entry.set_user_or_supervisor(test(flags, PageEntryFlags::Supervisor));
	entry.set_execute_disabled(test(flags,PageEntryFlags::ExecuteDisabled));
}

static bool check_alignment(LineAddr linaddr, PhysAddr phyaddr, PageSize ps)
{
	return 	(linaddr & (get_page_size_bytes(ps) - 1)) == 0 &&
		(phyaddr & (get_page_size_bytes(ps) - 1)) == 0;
}

static kstd::Maybe<PageSize> find_min_page_size(
		LineAddr linaddr, PhysAddr phyaddr, size_t size_limit)
{
	for (int i = num_page_sizes - 1; i >= 0; --i) {
		PageSize page_size = page_sizes[i];
		auto page_size_bytes = get_page_size_bytes(page_size);

		if (page_size_bytes > size_limit)
			continue;

		if ( 	(page_size_bytes > size_limit) 		||
			(linaddr & (page_size_bytes - 1)) 	||
			(phyaddr & (page_size_bytes - 1))
		)
			continue;
		return page_size;
	}

	return {};
}

}
