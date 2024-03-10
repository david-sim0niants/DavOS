#ifndef _LD__SEGMENT_H__
#define _LD__SEGMENT_H__

#include <ld/ldsym.h>

#ifndef SEGMENT_ALIGNMENT
#define SEGMENT_ALIGNMENT 4K
#endif

#define SEGMENT(name) __kernel_segment__##name
#define SEGMENT_VMA(name) __kernel_segment__##name##_vma
#define SEGMENT_LMA(name) __kernel_segment__##name##_lma
#define SEGMENT_SIZE(name) __kernel_segment__##name##_size

#define DEFINE_SEGMENT(name, ...) \
	. = ALIGN(SEGMENT_ALIGNMENT); \
	DEFINE_SEGMENT_START(name, . - __tot_hole_size, .) \
	__VA_ARGS__ \
	DEFINE_SEGMENT_SIZE(name, . - SEGMENT_VMA(name))

#define DEFINE_SEGMENT_START(name, lma, vma) \
	. = ALIGN(SEGMENT_ALIGNMENT); \
	SEGMENT_VMA(name) = vma; \
	SEGMENT_LMA(name) = lma; \

#define DEFINE_SEGMENT_SIZE(name, size) \
	. = ALIGN(SEGMENT_ALIGNMENT); \
	SEGMENT_SIZE(name) = size; \

#define SET_HOLE(size) \
	PROVIDE(__tot_hole_size = size);

#define ADD_HOLE(delta_size) \
	__tot_hole_size += delta_size;

#define DEFINE_SEGMENT_LDSYMS(name)\
	DEFINE_LDSYM(kernel_segment__##name##_vma) \
	DEFINE_LDSYM(kernel_segment__##name##_lma) \
	DEFINE_LDSYM(kernel_segment__##name##_size)

#define DEFINE_SEGMENT_LDSYMS_NULL(name)\
	DEFINE_LDSYM_NULL(kernel_segment__##name##_vma) \
	DEFINE_LDSYM_NULL(kernel_segment__##name##_lma) \
	DEFINE_LDSYM_NULL(kernel_segment__##name##_size)

#endif
