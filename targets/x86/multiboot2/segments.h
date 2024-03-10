#ifndef _LD__SEGMENT_H__
#define _LD__SEGMENT_H__

#include <kstd/array.h>
#include <kstd/algorithm.h>

#include <kernel_segment.h>

#include <x86/ldsym.h>

namespace kernel_image {

DECLARE_SEGMENT(i386_text)
DECLARE_SEGMENT(i386_data)
DECLARE_SEGMENT(i386_rodata)
DECLARE_SEGMENT(stack)

MAKE_SEGMENT_GLOBAL(i386_text, SegmentFlag::Read | SegmentFlag::Executable)
MAKE_SEGMENT_GLOBAL(i386_data, SegmentFlag::Read | SegmentFlag::Write)
MAKE_SEGMENT_GLOBAL(i386_rodata, SegmentFlag::Read)
MAKE_SEGMENT_GLOBAL(stack, SegmentFlag::Read | SegmentFlag::Write)

#ifndef __KERNEL_ENTRY__
#if CONFIG_ARCH == ARCH_x86_64
	DECLARE_SEGMENT(x86_64_text)
	DECLARE_SEGMENT(x86_64_data)
	DECLARE_SEGMENT(x86_64_rodata)
#endif
DECLARE_SEGMENT(text)
DECLARE_SEGMENT(data)
DECLARE_SEGMENT(rodata)

#if CONFIG_ARCH == ARCH_x86_64
	MAKE_SEGMENT_GLOBAL(x86_64_text, SegmentFlag::Read | SegmentFlag::Executable)
	MAKE_SEGMENT_GLOBAL(x86_64_data, SegmentFlag::Read | SegmentFlag::Write)
	MAKE_SEGMENT_GLOBAL(x86_64_rodata, SegmentFlag::Read)
#endif
MAKE_SEGMENT_GLOBAL(text, SegmentFlag::Read | SegmentFlag::Executable)
MAKE_SEGMENT_GLOBAL(data, SegmentFlag::Read | SegmentFlag::Write)
MAKE_SEGMENT_GLOBAL(rodata, SegmentFlag::Read)

const kstd::Array entry_segments = {
	&i386_text_segment,
	&i386_data_segment,
	&i386_rodata_segment,
	&stack_segment,
#if CONFIG_ARCH == ARCH_x86_64
	&x86_64_text_segment,
	&x86_64_data_segment,
	&x86_64_rodata_segment,
#endif
};

const kstd::Array main_segments = {
	&text_segment,
	&data_segment,
	&rodata_segment,
};

#endif


inline auto get_ldsym_entry_segments()
{
	return kstd::Array {
		LDSYM_Segment {SEGMENT_SYMS(i386_text), SegmentFlag::Read | SegmentFlag::Executable},
		LDSYM_Segment {SEGMENT_SYMS(i386_data), SegmentFlag::Read | SegmentFlag::Write},
		LDSYM_Segment {SEGMENT_SYMS(i386_rodata), SegmentFlag::Read},
		MAKE_SEGMENT_LDSYM(stack, SegmentFlag::Read | SegmentFlag::Write),
#if CONFIG_ARCH == ARCH_x86_64
		MAKE_SEGMENT_LDSYM(x86_64_text, SegmentFlag::Read | SegmentFlag::Executable),
		MAKE_SEGMENT_LDSYM(x86_64_data, SegmentFlag::Read | SegmentFlag::Write),
		MAKE_SEGMENT_LDSYM(x86_64_rodata, SegmentFlag::Read),
#endif
	};
}

inline auto get_ldsym_main_segments()
{
	return kstd::Array {
		MAKE_SEGMENT_LDSYM(text, SegmentFlag::Read | SegmentFlag::Executable),
		MAKE_SEGMENT_LDSYM(data, SegmentFlag::Read | SegmentFlag::Write),
		MAKE_SEGMENT_LDSYM(rodata, SegmentFlag::Read),
	};
}

inline auto get_ldsym_segments()
{
	auto entry_segments = get_ldsym_entry_segments();
	auto main_segments = get_ldsym_main_segments();
	kstd::Array<LDSYM_Segment, entry_segments.size() + main_segments.size()> segments;
	kstd::copy(entry_segments.data(), entry_segments.data() + entry_segments.size(),
			segments.data());
	kstd::copy(main_segments.data(), main_segments.data() + main_segments.size(),
			segments.data() + entry_segments.size());
	return segments;
}

}

#endif
