#ifndef _TARGETS_x86_MULTIBOOT2__SECTIONS_H__
#define _TARGETS_x86_MULTIBOOT2__SECTIONS_H__

#include <kstd/section.h>
#include <kstd/array.h>

#include <x86/ldsym.h>
#include <x86/addressing.h>

namespace kernel_image {

using Section = kstd::Section<x86::LineAddr, x86::PhysAddr, x86::PhysSize>;

inline auto get_sections()
{
	return kstd::Array
	{
		Section
		{
			".i386.text",
			(x86::LineAddr)__ldsym__kernel_i386_text_start_vma,
			(x86::PhysAddr)__ldsym__kernel_i386_text_start_lma,
			(x86::PhysSize)__ldsym__kernel_i386_text_size,
			kstd::SectionFlag::Executable | kstd::SectionFlag::Read,
		},
		Section
		{
			".i386.rodata",
			(x86::LineAddr)__ldsym__kernel_i386_rodata_start_vma,
			(x86::PhysAddr)__ldsym__kernel_i386_rodata_start_lma,
			(x86::PhysSize)__ldsym__kernel_i386_rodata_size,
			kstd::SectionFlag::Read,
		},
		Section
		{
			".i386.data",
			(x86::LineAddr)__ldsym__kernel_i386_data_start_vma,
			(x86::PhysAddr)__ldsym__kernel_i386_data_start_lma,
			(x86::PhysSize)__ldsym__kernel_i386_data_size,
			kstd::SectionFlag::Read | kstd::SectionFlag::Write,
		},
		Section
		{
			".ldsym",
			(x86::LineAddr)__ldsym__kernel_ldsym_start_vma,
			(x86::PhysAddr)__ldsym__kernel_ldsym_start_lma,
			(x86::PhysSize)__ldsym__kernel_ldsym_size,
			kstd::SectionFlag::Read,
		},
#if CONFIG_ARCH == ARCH_x86_64
		Section
		{
			".x86_64.text",
			(x86::LineAddr)__ldsym__kernel_x86_64_text_start_vma,
			(x86::PhysAddr)__ldsym__kernel_x86_64_text_start_lma,
			(x86::PhysSize)__ldsym__kernel_x86_64_text_size,
			kstd::SectionFlag::Read | kstd::SectionFlag::Executable,
		},
		Section
		{
			".x86_64.rodata",
			(x86::LineAddr)__ldsym__kernel_x86_64_rodata_start_vma,
			(x86::PhysAddr)__ldsym__kernel_x86_64_rodata_start_lma,
			(x86::PhysSize)__ldsym__kernel_x86_64_rodata_size,
			kstd::SectionFlag::Read,
		},
		Section
		{
			".x86_64.data",
			(x86::LineAddr)__ldsym__kernel_x86_64_data_start_vma,
			(x86::PhysAddr)__ldsym__kernel_x86_64_data_start_lma,
			(x86::PhysSize)__ldsym__kernel_x86_64_data_size,
			kstd::SectionFlag::Read | kstd::SectionFlag::Write,
		},
#endif
		Section
		{
			".stack",
			(x86::LineAddr)__ldsym__kernel_stack_start_vma,
			(x86::PhysAddr)__ldsym__kernel_stack_start_lma,
			(x86::PhysSize)__ldsym__kernel_stack_size,
			kstd::SectionFlag::Read | kstd::SectionFlag::Write,
		},
		Section
		{
			".text",
			(x86::LineAddr)__ldsym__kernel_text_start_vma,
			(x86::PhysAddr)__ldsym__kernel_text_start_lma,
			(x86::PhysSize)__ldsym__kernel_text_size,
			kstd::SectionFlag::Executable | kstd::SectionFlag::Read,
		},
		Section
		{
			".bss",
			(x86::LineAddr)__ldsym__kernel_bss_start_vma,
			(x86::PhysAddr)__ldsym__kernel_bss_start_lma,
			(x86::PhysSize)__ldsym__kernel_bss_size,
			kstd::SectionFlag::Read | kstd::SectionFlag::Write | kstd::SectionFlag::NoLoad,
		},
		Section
		{
			".rodata",
			(x86::LineAddr)__ldsym__kernel_rodata_start_vma,
			(x86::PhysAddr)__ldsym__kernel_rodata_start_lma,
			(x86::PhysSize)__ldsym__kernel_rodata_size,
			kstd::SectionFlag::Read,
		},
		Section
		{
			".data",
			(x86::LineAddr)__ldsym__kernel_data_start_vma,
			(x86::PhysAddr)__ldsym__kernel_data_start_lma,
			(x86::PhysSize)__ldsym__kernel_data_size,
			kstd::SectionFlag::Read | kstd::SectionFlag::Write,
		},
	};
};

}

#endif
