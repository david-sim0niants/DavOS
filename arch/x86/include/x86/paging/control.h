#ifndef _x86_PAGING__CONTROL_H__
#define _x86_PAGING__CONTROL_H__

#include <x86/config.h>
#include <x86/addressing.h>
#include <x86/cr.h>
#include <x86/system.h>

#include <stdint.h>
#include <compiler_attributes.h>

namespace x86 {

inline void set_curr_pt_ptr(PhysAddr pt_addr)
{
	write_cr3(pt_addr);
}

__FORCE_INLINE void enable_paging__common()
{
	auto cr0_val = read_cr0_flags();
	cr0_val |= CR0_Flags::WP | CR0_Flags::PG;
	write_cr0_flags(cr0_val);
}

__FORCE_INLINE void enable_paging_level_2()
{
	auto cr4_val = read_cr4_flags();
	cr4_val &= ~CR4_Flags::PAE;
	write_cr4_flags(cr4_val);

	enable_paging__common();
}

__FORCE_INLINE void enable_paging_level_3_PAE()
{
	auto cr4_val = read_cr4_flags();
	cr4_val |= CR4_Flags::PAE;
	write_cr4_flags(cr4_val);

	auto efer_val = read_efer_flags();
	efer_val &= ~EFER_Flags::LME;
	efer_val |= EFER_Flags::NXE;
	write_efer_flags(efer_val);

	enable_paging__common();
}

__FORCE_INLINE void enable_paging_level_4()
{
	auto cr4_val = read_cr4_flags();
	cr4_val |= CR4_Flags::PAE;
	cr4_val &= ~CR4_Flags::LA57;
	write_cr4_flags(cr4_val);

	auto efer_val = read_efer_flags();
	efer_val |= EFER_Flags::LME | EFER_Flags::NXE;
	write_efer_flags(efer_val);

	enable_paging__common();
}

__FORCE_INLINE void enable_paging_level_5()
{
	auto cr4_val = read_cr4_flags();
	cr4_val |= CR4_Flags::PAE | CR4_Flags::LA57;
	write_cr4_flags(cr4_val);

	auto efer_val = read_efer_flags();
	efer_val |= EFER_Flags::LME | EFER_Flags::NXE;
	write_efer_flags(efer_val);

	enable_paging__common();
}

__FORCE_INLINE void enable_paging()
{
#if CONFIG_x86_PAGE_MAP_LEVEL == x86_PAGE_MAP_LEVEL_2
	enable_paging_level_2();
#elif CONFIG_x86_PAGE_MAP_LEVEL == x86_PAGE_MAP_LEVEL_3_PAE
	enable_paging_level_3_PAE();
#elif CONFIG_x86_PAGE_MAP_LEVEL == x86_PAGE_MAP_LEVEL_3_PAE
	enable_paging_level_3_PAE();
#elif CONFIG_x86_PAGE_MAP_LEVEL == x86_PAGE_MAP_LEVEL_4
	enable_paging_level_4();
#elif CONFIG_x86_PAGE_MAP_LEVEL == x86_PAGE_MAP_LEVEL_5
	enable_paging_level_5();
#endif
}

}

#endif
