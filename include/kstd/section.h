#ifndef _KSTD__SECTION_H__
#define _KSTD__SECTION_H__

#include <stddef.h>
#include <stdint.h>

#include <kstd/enum.h>

namespace kstd {

enum class SectionFlag {
	None = 0,
	Executable = 1,
	Write = 2,
	Read = 4,
	NoLoad = 8,
};
KSTD_DEFINE_ENUM_LOGIC_BITWISE_OPERATORS(SectionFlag);

struct Section {
	const char *name;
	uintptr_t vma_start;
	uintptr_t lma_start;
	size_t size;
	SectionFlag flags;
};

}

#endif
