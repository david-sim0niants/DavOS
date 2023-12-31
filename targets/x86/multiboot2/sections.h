#ifndef _TARGETS_x86_MULTIBOOT2__SECTIONS_H__
#define _TARGETS_x86_MULTIBOOT2__SECTIONS_H__

#include <kstd/section.h>
#include <kstd/array.h>

#include <x86/ldsym.h>

namespace kernel_image {

inline kstd::Array<kstd::Section, 9> get_sections()
{
	kstd::Section sections[] = {
		{
			".i386.text",
			(uintptr_t)__ldsym__kernel_i386_text_start_vma,
			(uintptr_t)__ldsym__kernel_i386_text_start_lma,
			(size_t)__ldsym__kernel_i386_text_size,
			kstd::SectionFlag::Executable | kstd::SectionFlag::Read,
		},
		{
			".i386.rodata",
			(uintptr_t)__ldsym__kernel_i386_rodata_start_vma,
			(uintptr_t)__ldsym__kernel_i386_rodata_start_lma,
			(size_t)__ldsym__kernel_i386_rodata_size,
			kstd::SectionFlag::Read,
		},
		{
			".ldsym",
			(uintptr_t)__ldsym__kernel_ldsym_start_vma,
			(uintptr_t)__ldsym__kernel_ldsym_start_lma,
			(size_t)__ldsym__kernel_ldsym_size,
			kstd::SectionFlag::Read,
		},
		{
			".i386.data",
			(uintptr_t)__ldsym__kernel_i386_data_start_vma,
			(uintptr_t)__ldsym__kernel_i386_data_start_lma,
			(size_t)__ldsym__kernel_i386_data_size,
			kstd::SectionFlag::Read | kstd::SectionFlag::Write,
		},
		{
			".stack",
			(uintptr_t)__ldsym__kernel_stack_start_vma,
			(uintptr_t)__ldsym__kernel_stack_start_lma,
			(size_t)__ldsym__kernel_stack_size,
			kstd::SectionFlag::Read | kstd::SectionFlag::Write,
		},
		{
			".text",
			(uintptr_t)__ldsym__kernel_text_start_vma,
			(uintptr_t)__ldsym__kernel_text_start_lma,
			(size_t)__ldsym__kernel_text_size,
			kstd::SectionFlag::Executable | kstd::SectionFlag::Read,
		},
		{
			".bss",
			(uintptr_t)__ldsym__kernel_bss_start_vma,
			(uintptr_t)__ldsym__kernel_bss_start_lma,
			(size_t)__ldsym__kernel_bss_size,
			kstd::SectionFlag::Read | kstd::SectionFlag::Write | kstd::SectionFlag::NoLoad,
		},
		{
			".rodata",
			(uintptr_t)__ldsym__kernel_rodata_start_vma,
			(uintptr_t)__ldsym__kernel_rodata_start_lma,
			(size_t)__ldsym__kernel_rodata_size,
			kstd::SectionFlag::Read,
		},
		{
			".data",
			(uintptr_t)__ldsym__kernel_data_start_vma,
			(uintptr_t)__ldsym__kernel_data_start_lma,
			(size_t)__ldsym__kernel_data_size,
			kstd::SectionFlag::Read | kstd::SectionFlag::Write,
		},
	};
	return kstd::Array<kstd::Section, sizeof(sections) / sizeof(sections[0])>(sections);
};

}

#endif
