#include <x86/paging/page_map.h>
#include <kstd/overflow.h>
#include <kstd/either.h>
#include <string.h>


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

template<int pml> typename PageTable_<pml>::
Err PageTable_<pml>::map_page__no_mm(LineAddr linaddr, PhysAddr phyaddr,
	PageSize ps, int flags, uintptr_t &free_mem_beg, uintptr_t free_mem_end)
{
	auto map_page__static = [this, linaddr, phyaddr, flags,
		&free_mem_beg, free_mem_end] <PageSize ps>
	{
		return map_page__no_mm_internal<ps>(
			linaddr, phyaddr, flags, free_mem_beg, free_mem_end);
	};

	switch (ps) {
	case PageSize::PS_4Kb:
		return map_page__static.template operator()<PageSize::PS_4Kb>();
#if CONFIG_x86_PAGE_MAP_LEVEL >= x86_PAGE_MAP_LEVEL_3_PA
	case PageSize::PS_2Mb:
		return map_page__static.template operator()<PageSize::PS_2Mb>();
#endif
#if CONFIG_x86_PAGE_MAP_LEVEL == x86_PAGE_MAP_LEVEL_2
	case PageSize::PS_4Mb:
		return map_page__static.template operator()<PageSize::PS_4Mb>();
#endif
#if CONFIG_x86_PAGE_MAP_LEVEL >= x86_PAGE_MAP_LEVEL_4
	case PageSize::PS_1Gb:
		return map_page__static.template operator()<PageSize::PS_1Gb>();
#endif
	};

	return Err::NONE;
}

template<int pml> template<PageSize page_size> typename PageTable_<pml>::
Err PageTable_<pml>::map_page__no_mm_internal(
	LineAddr linaddr, PhysAddr phyaddr, int flags,
	uintptr_t &free_mem_beg, uintptr_t free_mem_end)
{
	static constexpr
	auto CONTROLLED_MEM_REGION =
		LineAddr(1) << PageTableEntry_<pml>::CONTROLLED_BITS;

	static_assert((unsigned)page_size <= CONTROLLED_MEM_REGION,
		"Page size larger than the controlled memory region at the "
		"current page map level.\n"
		"NOTE: it was probably smaller than the controlled memory "
		"region at the above page map level which means it's an "
		"invalid value.");

	PageTableEntry_<pml> &entry = entries[get_pte_idx<pml>(linaddr)];

	if constexpr ((unsigned)page_size == CONTROLLED_MEM_REGION) {
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

template<int pml> typename PageTable_<pml>::
Err PageTable_<pml>::map_page_range__no_mm(LineAddr linaddr, PhysAddr phyaddr,
	size_t range_size, int flags,
	uintptr_t free_mem_beg, uintptr_t free_mem_end)
{
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

}
