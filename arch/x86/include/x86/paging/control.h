#ifndef _x86_PAGING__CONTROL_H__
#define _x86_PAGING__CONTROL_H__

#include <x86/config.h>
#include <x86/addressing.h>
#include <x86/cr.h>
#include <stdint.h>
#include <compiler_attributes.h>

namespace x86 {

inline void set_curr_pt_ptr(PhysAddr pt_addr)
{
	write_cr3(pt_addr);
}

inline void enable_paging__common()
{
	unsigned long cr0_val = read_cr0();
	cr0_val |= CR0_Flag::WP | CR0_Flag::PG;
	write_cr0(cr0_val);
}

inline void enable_paging_level_2()
{
	unsigned long cr4_val = read_cr4();
	cr4_val &= ~CR4_Flag::PAE;
	write_cr4(cr4_val);

	enable_paging__common();
}

inline void enable_paging_level_3_PAE()
{
	unsigned long cr4_val = read_cr4();
	cr4_val |= CR4_Flag::PAE;
	write_cr4(cr4_val);

	unsigned long efer_val = read_efer();
	efer_val &= ~EFER_Flag::LME;
	write_efer(efer_val);

	enable_paging__common();
}

inline void enable_paging_level_4()
{
	unsigned long cr4_val = read_cr4();
	cr4_val |= CR4_Flag::PAE;
	cr4_val &= ~CR4_Flag::LA57;
	write_cr4(cr4_val);

	unsigned long efer_val = read_efer();
	efer_val |= EFER_Flag::LME;
	write_efer(efer_val);

	enable_paging__common();
}

inline void enable_paging_level_5()
{
	unsigned long cr4_val = read_cr4();
	cr4_val |= CR4_Flag::PAE | CR4_Flag::LA57;
	write_cr4(cr4_val);

	unsigned long efer_val = read_efer();
	efer_val |= EFER_Flag::LME;
	write_efer(efer_val);

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
