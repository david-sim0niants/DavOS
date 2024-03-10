#ifndef __KERNEL_SEGMENT_H__
#define __KERNEL_SEGMENT_H__

#include <stddef.h>
#include <stdint.h>

#include <kstd/enum.h>

#define DECLARE_SEGMENT(name) extern "C" int __kernel_segment__##name##_vma, \
	__kernel_segment__##name##_lma, __kernel_segment__##name##_size;

#define SEGMENT_VMA(name) (uintptr_t)(&__kernel_segment__##name##_vma)
#define SEGMENT_LMA(name) (uintptr_t)(&__kernel_segment__##name##_lma)
#define SEGMENT_SIZE(name) (size_t)(&__kernel_segment__##name##_size)
#define SEGMENT_SYMS(name) SEGMENT_VMA(name), SEGMENT_LMA(name), SEGMENT_SIZE(name)

#define SEGMENT_VMA_LDSYM(name) __ldsym__kernel_segment__##name##_vma
#define SEGMENT_LMA_LDSYM(name) __ldsym__kernel_segment__##name##_lma
#define SEGMENT_SIZE_LDSYM(name) __ldsym__kernel_segment__##name##_size
#define SEGMENT_LDSYMS(name) SEGMENT_VMA_LDSYM(name), SEGMENT_LMA_LDSYM(name), SEGMENT_SIZE_LDSYM(name)


#define MAKE_SEGMENT(name, flags) (Segment<> {SEGMENT_SYMS(name), (flags)})
#define MAKE_SEGMENT_GLOBAL(name, flags) const auto name##_segment = MAKE_SEGMENT(name, flags);

#define MAKE_SEGMENT_LDSYM(name, flags) (LDSYM_Segment {SEGMENT_LDSYMS(name), (flags)})

namespace kernel_image {

enum class SegmentFlag {
	None = 0,
	Executable = 1,
	Write = 2,
	Read = 4,
};
KSTD_DEFINE_ENUM_LOGIC_BITWISE_OPERATORS(SegmentFlag);

template <typename VMAType = uintptr_t, typename LMAType = uintptr_t, typename SizeType = size_t>
struct Segment {
	VMAType vma_start;
	LMAType lma_start;
	SizeType size;
	SegmentFlag flags;
};

using LDSYM_Segment = Segment<uint64_t, uint64_t, uint64_t>;

}

#endif
