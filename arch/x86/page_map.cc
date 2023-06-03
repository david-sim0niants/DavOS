#include <x86/paging/page_map.h>
#include <kstd/overflow.h>
#include <kstd/either.h>
#include <kstd/maybe.h>
#include <string.h>
#include <compiler_attributes.h>


namespace x86 {

enum class LocalErr {
	NONE = 0,
	OVERFLOW,
	NO_FREE_MEM,
};

static LocalErr align_address_to_floor(unsigned long &addr, size_t alignment);

template<int pml>
static kstd::Either<PhysAddr, LocalErr> create_page_table(
		uintptr_t free_mem_beg, uintptr_t free_mem_end);

static bool check_alignment(LineAddr linaddr, PhysAddr phyaddr, PageSize ps);
static kstd::Maybe<PageSize> find_min_page_size(
		LineAddr linaddr, PhysAddr phyaddr, size_t size_limit);


template<int pml> typename PageTable_<pml>::
Err PageTable_<pml>::map_memory__no_mm(LineAddr linaddr, PhysAddr phyaddr,
	size_t mem_size, int flags,
	uintptr_t &free_mem_beg, uintptr_t free_mem_end)
{
	if (kstd::add_will_overflow(linaddr, mem_size))
		return Err::LINEADDR_OVERFLOW;
	if (kstd::add_will_overflow(phyaddr, mem_size))
		return Err::PHYSADDR_OVERFLOW;

	while (mem_size > 0) {
		const auto maybe_page_size =
			find_min_page_size(linaddr, phyaddr, mem_size);
		if (!maybe_page_size.has_value())
			return Err::ADDRESS_MISMATCH;
		auto page_size = *maybe_page_size;
		auto page_size_bytes = get_page_size_bytes(page_size);
		auto page_size_shift = get_page_size_shift(page_size);

		auto nr_pages = mem_size >> page_size_shift;
		auto e = map_pages__no_mm(linaddr, phyaddr, nr_pages,
				page_size, flags, free_mem_beg, free_mem_end);
		if (e != Err::NONE)
			return e;
		mem_size -= nr_pages << page_size_shift;
	}

	return Err::NONE;
}

template<int pml> typename PageTable_<pml>::
Err PageTable_<pml>::map_pages__no_mm(LineAddr linaddr, PhysAddr phyaddr,
		size_t nr_pages, PageSize page_size, int flags,
		uintptr_t &free_mem_beg, uintptr_t free_mem_end)
{
	if (!check_alignment(linaddr, phyaddr, page_size))
		return Err::UNALIGNED_ADDRESS;

	return map_pages__no_mm_no_chk(linaddr, phyaddr, nr_pages,
			page_size, flags, free_mem_beg, free_mem_end);
}

template<int pml> typename PageTable_<pml>::
Err PageTable_<pml>::map_page__no_mm(LineAddr linaddr, PhysAddr phyaddr,
		PageSize page_size, int flags,
		uintptr_t &free_mem_beg, uintptr_t free_mem_end)
{
	if (!check_alignment(linaddr, phyaddr, page_size))
		return Err::UNALIGNED_ADDRESS;

	return map_page__no_mm_no_chk(linaddr, phyaddr, page_size, flags,
			free_mem_beg, free_mem_end);
}

template<int pml> typename PageTable_<pml>::
Err PageTable_<pml>::map_pages__no_mm_no_chk(LineAddr linaddr, PhysAddr phyaddr,
		size_t nr_pages, PageSize page_size, int flags,
		uintptr_t &free_mem_beg, uintptr_t free_mem_end)
{
	auto ps_bytes = get_page_size_bytes(page_size);
	for (size_t i = 0; i < nr_pages; ++i) {

		auto e = map_page__no_mm_no_chk(linaddr, phyaddr, page_size,
				flags, free_mem_beg, free_mem_end);
		if (e != Err::NONE)
			return e;

		linaddr += ps_bytes;
		phyaddr += ps_bytes;
	}

	return Err::NONE;
}

template<int pml> typename PageTable_<pml>::
Err PageTable_<pml>::map_page__no_mm_no_chk(LineAddr linaddr, PhysAddr phyaddr,
	PageSize ps, int flags, uintptr_t &free_mem_beg, uintptr_t free_mem_end)
{
	auto map_page__static = [this, linaddr, phyaddr, flags,
		&free_mem_beg, free_mem_end] <PageSize ps>
	{
		return map_page__no_mm_internal<ps>(
			linaddr, phyaddr, flags, free_mem_beg, free_mem_end);
	};

	switch (ps) {
	case PageSize::_4Kb:
		return map_page__static.template operator()<PageSize::_4Kb>();
#if CONFIG_x86_PAGE_MAP_LEVEL >= x86_PAGE_MAP_LEVEL_3_PAE
	case PageSize::_2Mb:
		return map_page__static.template operator()<PageSize::_2Mb>();
#endif
#if CONFIG_x86_PAGE_MAP_LEVEL == x86_PAGE_MAP_LEVEL_2
	case PageSize::_4Mb:
		return map_page__static.template operator()<PageSize::_4Mb>();
#endif
#if CONFIG_x86_PAGE_MAP_LEVEL >= x86_PAGE_MAP_LEVEL_4
	case PageSize::_1Gb:
		return map_page__static.template operator()<PageSize::_1Gb>();
#endif
	};

	return Err::NONE;
}

template<int pml> template<PageSize page_size>
__FORCE_INLINE typename PageTable_<pml>::
Err PageTable_<pml>::map_page__no_mm_internal(
	LineAddr linaddr, PhysAddr phyaddr, int flags,
	uintptr_t &free_mem_beg, uintptr_t free_mem_end)
{
	static constexpr
	auto CONTROLLED_BITS = PageTableEntry_<pml>::CONTROLLED_BITS;

	constexpr auto page_size_shift = get_page_size_shift(page_size);

	static_assert(page_size_shift <= CONTROLLED_BITS,
		"Page size larger than the controlled memory region at the "
		"current page map level.\n"
		"NOTE: it was probably smaller than the controlled memory "
		"region at the above page map level which means it's an "
		"invalid value.");

	PageTableEntry_<pml> &entry = entries[get_pte_idx<pml>(linaddr)];

	if constexpr (page_size_shift == CONTROLLED_BITS) {
		if (entry.is_present())
			return Err::EXISTING_PAGE_MAP;
		else
			entry.set_present(true);
		entry.map_page(phyaddr, flags & PAGE_ENTRY_GLOBAL);
	} else {
		static_assert(pml > 1, "Can't have pml == 1 here.");
		if (entry.maps_page())
			return Err::EXISTING_PAGE_MAP;

		PhysAddr next_pt_addr;
		if (entry.maps_page_table()) {
			next_pt_addr = entry.get_page_table_addr();
		} else {
			auto maybe_pt_ptr = create_page_table<pml - 1>(
				free_mem_beg, free_mem_end);

			auto *p_err = kstd::try_get<LocalErr>(maybe_pt_ptr);
			if (p_err && *p_err == LocalErr::NO_FREE_MEM)
				return Err::NO_FREE_MEM;

			next_pt_addr = kstd::get<PhysAddr>(maybe_pt_ptr);
			entry.map_page_table((PhysAddr)next_pt_addr);
		}

		auto next_pt_ptr = reinterpret_cast<
			PageTable_<pml - 1> *>(next_pt_addr);
		next_pt_ptr->template map_page__no_mm_internal<page_size>(
			linaddr, phyaddr, flags,
			free_mem_beg, free_mem_end);
	}

	entry.set_write_allowed(flags & PAGE_ENTRY_WRITE_ALLOWED);
	entry.set_user_or_supervisor(flags & PAGE_ENTRY_SUPERVISOR);
	entry.set_execute_disabled(flags & PAGE_ENTRY_EXECUTE_DISABLED);

	return Err::NONE;
}

template class PageTable_<MAX_PAGE_MAP_LEVEL>;


static LocalErr align_address_to_floor(unsigned long &addr, size_t alignment)
{
	const auto prev_addr = addr;
	addr = (addr / alignment) * alignment;
	if (addr < prev_addr) [[likely]] {
		if (kstd::add_overflow(addr, alignment)) [[unlikely]]
			return LocalErr::OVERFLOW;
	}
	return LocalErr::NONE;
}

template<int pml>
static kstd::Either<PhysAddr, LocalErr> create_page_table(
		uintptr_t free_mem_beg, uintptr_t free_mem_end)
{
	auto constexpr PT_SIZE = PageTable_<pml>::SIZE;
	auto e = align_address_to_floor(free_mem_beg, PT_SIZE);
	if (e == LocalErr::OVERFLOW) [[unlikely]]
		return LocalErr::NO_FREE_MEM;

	auto new_pt_ptr = free_mem_beg;
	if (kstd::add_overflow(free_mem_beg, PT_SIZE)) [[unlikely]]
		return LocalErr::NO_FREE_MEM;

	if (free_mem_beg > free_mem_end)
		return LocalErr::NO_FREE_MEM;

	memset((void *)new_pt_ptr, 0, PageTable_<pml>::SIZE);
	return (PhysAddr)new_pt_ptr;
}

static bool check_alignment(LineAddr linaddr, PhysAddr phyaddr, PageSize ps)
{
	return 	(linaddr & ((1 << get_page_size_shift(ps)) - 1)) == 0 &&
		(phyaddr & ((1 << get_page_size_shift(ps)) - 1)) == 0;

}

static kstd::Maybe<PageSize> find_min_page_size(
		LineAddr linaddr, PhysAddr phyaddr, size_t size_limit)
{
	for (int i = NUM_PAGE_SIZES - 1; i >= 0; --i) {
		PageSize page_size = PAGE_SIZES[i];
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
