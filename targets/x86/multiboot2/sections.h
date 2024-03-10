#ifndef _TARGETS_x86_MULTIBOOT2__SECTIONS_H__
#define _TARGETS_x86_MULTIBOOT2__SECTIONS_H__

#include <kstd/array.h>

#include <kernel_section.h>

#include <x86/ldsym.h>

namespace kernel_image {

DECLARE_SECTION(i386_text)
DECLARE_SECTION(i386_data)
DECLARE_SECTION(i386_rodata)
DECLARE_SECTION(ldsym)
DECLARE_SECTION(stack)

MAKE_SECTION_GLOBAL(i386_text, ".i386.text", SectionFlag::Read | SectionFlag::Executable)
MAKE_SECTION_GLOBAL(i386_data, ".i386.data", SectionFlag::Read | SectionFlag::Write)
MAKE_SECTION_GLOBAL(i386_rodata, ".i386.rodata", SectionFlag::Read)
MAKE_SECTION_GLOBAL(ldsym, ".ldsym", SectionFlag::Read)
MAKE_SECTION_GLOBAL(stack, ".stack", SectionFlag::Read | SectionFlag::Write)

#ifndef __KERNEL_ENTRY__

#if CONFIG_ARCH == ARCH_x86_64
DECLARE_SECTION(x86_64_text)
DECLARE_SECTION(x86_64_data)
DECLARE_SECTION(x86_64_rodata)

MAKE_SECTION_GLOBAL(x86_64_text, ".x86_64.text", SectionFlag::Read | SectionFlag::Executable)
MAKE_SECTION_GLOBAL(x86_64_data, ".x86_64.data", SectionFlag::Read | SectionFlag::Write)
MAKE_SECTION_GLOBAL(x86_64_rodata, ".x86_64.rodata", SectionFlag::Read)
#endif

DECLARE_SECTION(text)
DECLARE_SECTION(data)
DECLARE_SECTION(bss)
DECLARE_SECTION(rodata)
DECLARE_SECTION(init_array)

MAKE_SECTION_GLOBAL(text, ".text", SectionFlag::Read | SectionFlag::Executable)
MAKE_SECTION_GLOBAL(data, ".data", SectionFlag::Read | SectionFlag::Write)
MAKE_SECTION_GLOBAL(bss, ".bss", SectionFlag::Read | SectionFlag::Write)
MAKE_SECTION_GLOBAL(rodata, ".rodata", SectionFlag::Read)
MAKE_SECTION_GLOBAL(init_array, ".init_array", SectionFlag::Read)

const kstd::Array entry_sections = {
	&i386_text_section,
	&i386_data_section,
	&i386_rodata_section,
	&ldsym_section,
	&stack_section,
#if CONFIG_ARCH == ARCH_x86_64
	&x86_64_text_section,
	&x86_64_data_section,
	&x86_64_rodata_section,
#endif
};

const kstd::Array main_sections = {
	&text_section,
	&data_section,
	&bss_section,
	&rodata_section,
	&init_array_section,
};

#endif

}

#endif
