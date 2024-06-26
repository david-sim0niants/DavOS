#include <x86/paging/page_map.h>

#include <string.h>
#include <compiler_attributes.h>

#include <kstd/overflow.h>
#include <kstd/either.h>
#include <kstd/algorithm.h>
#include <kstd/new.h>

namespace x86 {

enum class LocalErr {
	None = 0,
	Overflow,
	NoFreeMemory,
};

template<int pml>
static kstd::Either<PhysAddr, LocalErr> create_page_table(kstd::MemoryRange& free_mem);

enum class SetEntryFlagsMode {
	Page, PageTable
};

/* Sets entry flags for page table entry. If the 'mode' template argument is Page
 * will set the flags for lowest level page table entry.
 * Otherwise will do it for an entry that maps another page table. */
template<int pml, SetEntryFlagsMode mode>
static void set_entry_flags(PageTableEntry_<pml>& entry, PageEntryFlags flags);

static bool check_alignment(LineAddr linaddr_beg, PhysAddr phyaddr_beg, PageSize ps);

template<int pml> PageMapErr PageTable_<pml>::map_memory(
		PageMappingInfo &info, kstd::MemoryRange& free_mem)
{
	auto e = check_overflow(info.linaddr_beg, info.phyaddr_beg, info.phyaddr_end);
	if (e != PageMapErr::None)
		return e;

	while (info.phyaddr_beg < info.phyaddr_end) {
		// get max page size that linaddr_beg and phyaddr_beg are divisble by
		// and which is not larger than mapping size (phyaddr_end - phyaddr_beg)
		const auto maybe_page_size =
			find_max_page_size(info.linaddr_beg, info.phyaddr_beg,
					info.phyaddr_end - info.phyaddr_beg);
		if (!maybe_page_size.has_value())
			return PageMapErr::AddressMismatch;

		info.page_size = *maybe_page_size;
		auto e = map_pages__no_chk(info, free_mem);
		if (e != PageMapErr::None)
			return e;
	}

	return PageMapErr::None;
}

template<int pml> PageMapErr PageTable_<pml>::map_pages(
		PageMappingInfo& info, kstd::MemoryRange& free_mem)
{
	if (!check_alignment(info.linaddr_beg, info.phyaddr_beg, info.page_size))
		return PageMapErr::UnalignedAddress;

	const auto page_size_shift = get_page_size_shift(info.page_size);

	auto e = check_overflow(info.linaddr_beg, info.phyaddr_beg, info.phyaddr_end);
	if (e != PageMapErr::None)
		return e;

	return map_pages__no_chk(info, free_mem);
}

template<int pml> PageMapErr PageTable_<pml>::map_pages__no_chk(
		PageMappingInfo& info, kstd::MemoryRange& free_mem)
{
	switch (info.page_size) {
	case PageSize::_4Kb:
		return map_pages__const_ps<PageSize::_4Kb>(info, free_mem);
#if CONFIG_x86_PAGE_MAP_LEVEL >= x86_PAGE_MAP_LEVEL_3_PAE
	case PageSize::_2Mb:
		return map_pages__const_ps<PageSize::_2Mb>(info, free_mem);
#endif
#if CONFIG_x86_PAGE_MAP_LEVEL == x86_PAGE_MAP_LEVEL_2
	case PageSize::_4Mb:
		return map_pages__const_ps<PageSize::_4Mb>(info, free_mem);
#endif
#if CONFIG_x86_PAGE_MAP_LEVEL >= x86_PAGE_MAP_LEVEL_4
	case PageSize::_1Gb:
		return map_pages__const_ps<PageSize::_1Gb>(info, free_mem);
#endif
	};

	return PageMapErr::None;
}

template<int pml> template<PageSize page_size>
__FORCE_INLINE PageMapErr PageTable_<pml>::map_pages__const_ps(
		PageMappingInfo& info, kstd::MemoryRange& free_mem)
{
	using kstd::test_flag;

	constexpr auto controlled_bits = PageTableEntry_<pml>::controlled_bits;
	constexpr auto page_size_shift = get_page_size_shift(page_size);

	static_assert(page_size_shift <= controlled_bits,
		"Page size larger than the controlled memory region at the "
		"current page map level.\n"
		"NOTE: it was probably smaller than the controlled memory "
		"region at the above page map level which means it had an "
		"invalid value.");

	if constexpr (page_size_shift == controlled_bits) { // mapping pages
		// if page size shift and number of bits under entries' control are the same,
		// it means we reached the lowest level page table

		// page size in bytes
		constexpr auto page_size_nb = get_page_size_bytes(page_size);
		// error value to return from this scope
		PageMapErr e = PageMapErr::None;

		// initial page table entry index
		auto pte_idx = get_pte_idx<pml>(info.linaddr_beg);
		auto initial_pte_idx = pte_idx;

		// map a page for each entry
		while (info.phyaddr_beg < info.phyaddr_end && pte_idx < nr_entries) {
			auto& entry = entries[pte_idx];
			if (entry.is_present()) { // the entry should not have a mapping yet
				e = PageMapErr::ExistingPageMap;
				break;
			}

			const bool global_flag = test_flag(info.flags, PageEntryFlags::Global);
			// map the given page entry to page physical address
			entry.map_page(info.phyaddr_beg, global_flag);

			set_entry_flags<SetEntryFlagsMode::Page>(entry, info.flags);
			entry.set_present(true);

			// update physical address and page table entry index
			info.phyaddr_beg += page_size_nb;
			++pte_idx;
		}

		// a small optimization, linear address doesn't have to be updated after each
		// loop iteration, so we update it only once here summarizing all the iterations
		info.linaddr_beg += (pte_idx - initial_pte_idx) * page_size_nb;
		return e;
	} else { // mapping a page table
		static_assert(pml > 1, "Can't have pml == 1 here.");

		// initial page table entry index
		auto pte_idx = get_pte_idx<pml>(info.linaddr_beg);

		while (info.phyaddr_end > info.phyaddr_beg && pte_idx < nr_entries) {
			PageTableEntry_<pml>& entry = entries[pte_idx];

			// if there's already a page table mapped by the given entry, get it,
			// otherwise create and get it
			auto next_pt_or_err = get_or_map_page_table(entry, free_mem);
			if (auto *e = kstd::try_get<PageMapErr>(next_pt_or_err))
				return *e;

			set_entry_flags<SetEntryFlagsMode::PageTable>(entry, info.flags);

			auto *next_pt = kstd::get<PageTable_<pml - 1> *>(next_pt_or_err);
			// "compile-time resolved recursive" call to the lower level page table
			/* As this function is marked __FORCE_INLINE, this call will actually
			 * be inlined here. There are no other places where this method of
			 * (pml < max_page_map_level) page table class could be called. */
			auto e = next_pt->template
				map_pages__const_ps<page_size>(info, free_mem);
			if (e != PageMapErr::None)
				return e;

			++pte_idx;
		}
	}

	return PageMapErr::None;
}

template<int pml> PageMapErr
PageTable_<pml>::check_overflow(LineAddr linaddr_beg, PhysAddr phyaddr_beg, PhysAddr phyaddr_end)
{
	if (phyaddr_beg > phyaddr_end
	 || phyaddr_end >= (PhysAddr(1)<< constants::max_possible_phyaddr_bits))
		return PageMapErr::PhysicalAddressOverflow;

	LineAddr linaddr_end;
	if (kstd::add_overflow(linaddr_beg, phyaddr_end - phyaddr_beg, linaddr_end)
	 || linaddr_end >= controlled_mem)
		return PageMapErr::LinearAddressOverflow;

	return PageMapErr::None;
}

template<int pml> kstd::Either<PageTable_<pml - 1> *, PageMapErr>
PageTable_<pml>::get_or_map_page_table(PageTableEntry_<pml>& entry, kstd::MemoryRange& free_mem)
{
	static_assert(pml > 1, "Can't have pml == 1 here.");
	PhysAddr next_pt_addr {};

	if (entry.is_present()) {
		if (entry.maps_page())
			return PageMapErr::ExistingPageMap;
		// if reached here then the entry does map a page table
		next_pt_addr = entry.get_page_table_addr();
	} else {
		// no page table pointed by the entry, creating
		auto maybe_pt_ptr = create_page_table<pml - 1>(free_mem);

		auto *p_err = kstd::try_get<LocalErr>(maybe_pt_ptr);
		if (p_err && *p_err == LocalErr::NoFreeMemory)
			return PageMapErr::NoFreeMem;

		next_pt_addr = kstd::get<PhysAddr>(maybe_pt_ptr);
		entry.map_page_table((PhysAddr)next_pt_addr);
		entry.set_present(true);
		entry.set_write_allowed(false);
		entry.set_supervisor(true);
		entry.set_execute_disabled(true);
	}

	return reinterpret_cast<PageTable_<pml - 1> *>(next_pt_addr);
}

/* Define the page table class. */
template class PageTable_<max_page_map_level>;


template<int pml>
static kstd::Either<PhysAddr, LocalErr> create_page_table(kstd::MemoryRange& free_mem)
{
	uintptr_t free_mem_beg = reinterpret_cast<uintptr_t>(free_mem.beg);
	free_mem_beg = kstd::align_ceiled(free_mem_beg, PageTable_<pml>::size_shift);
	if (free_mem_beg < (uintptr_t)free_mem.beg) [[unlikely]]
		return LocalErr::Overflow;

	PhysAddr new_pt_addr = static_cast<PhysAddr>(free_mem_beg);

	if (kstd::add_overflow(free_mem_beg, PageTable_<pml>::size)) [[unlikely]]
		return LocalErr::NoFreeMemory;

	free_mem.beg = reinterpret_cast<kstd::Byte *>(free_mem_beg);
	if (free_mem.beg >= free_mem.end)
		return LocalErr::NoFreeMemory;

	new ((void *)new_pt_addr) PageTable_<pml>();
	return new_pt_addr;
}

template<SetEntryFlagsMode mode, int pml>
static void set_entry_flags(PageTableEntry_<pml>& entry, PageEntryFlags flags)
{
	using kstd::test_flag;

	if constexpr (mode == SetEntryFlagsMode::Page) {
		// in page mode set flags unconditionally
		entry.set_write_allowed(
				test_flag(flags, PageEntryFlags::WriteAllowed));
		entry.set_supervisor(
				test_flag(flags, PageEntryFlags::Supervisor));
		entry.set_execute_disabled(
				test_flag(flags, PageEntryFlags::ExecuteDisabled));
	} else {
		// in page table mode set the most permissive flags
		entry.set_write_allowed(entry.is_write_allowed() ||
				test_flag(flags, PageEntryFlags::WriteAllowed));
		entry.set_supervisor(entry.is_supervisor() &&
				test_flag(flags, PageEntryFlags::Supervisor));
		entry.set_execute_disabled(entry.is_execute_disabled() &&
				test_flag(flags, PageEntryFlags::ExecuteDisabled));
	}
}

static bool check_alignment(LineAddr linaddr_beg, PhysAddr phyaddr_beg, PageSize ps)
{
	return 	(linaddr_beg & (get_page_size_bytes(ps) - 1)) == 0 &&
		(phyaddr_beg & (get_page_size_bytes(ps) - 1)) == 0;
}

}
