#ifndef __KERNEL_SECTION_H__
#define __KERNEL_SECTION_H__

#include <stddef.h>
#include <stdint.h>

#include <kstd/enum.h>

#define DECLARE_SECTION(name) extern "C" int __kernel_section__##name##_vma, \
	__kernel_section__##name##_lma, __kernel_section__##name##_size;

#define SECTION_VMA(name) (uintptr_t)(&__kernel_section__##name##_vma)
#define SECTION_LMA(name) (uintptr_t)(&__kernel_section__##name##_lma)
#define SECTION_SIZE(name) (size_t)(&__kernel_section__##name##_size)
#define SECTION_SYMS(name) SECTION_VMA(name), SECTION_LMA(name), SECTION_SIZE(name)

#define MAKE_SECTION(id_name, name, flags) Section<> {name, SECTION_SYMS(id_name), (flags)}
#define MAKE_SECTION_GLOBAL(id_name, name, flags)\
	const auto id_name##_section = MAKE_SECTION(id_name, name, flags);

namespace kernel_image {

enum class SectionFlag {
	None = 0,
	Executable = 1,
	Write = 2,
	Read = 4,
	NoLoad = 8,
};
KSTD_DEFINE_ENUM_LOGIC_BITWISE_OPERATORS(SectionFlag);

template <typename VMAType = uintptr_t, typename LMAType = uintptr_t, typename SizeType = size_t>
struct Section {
	const char *name;
	VMAType vma_start;
	LMAType lma_start;
	SizeType size;
	SectionFlag flags;
};

}

#endif
