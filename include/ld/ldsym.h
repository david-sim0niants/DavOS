#ifndef _LD__LDSYM_H__
#define _LD__LDSYM_H__

#ifndef SECTION_ALIGNMENT
#define SECTION_ALIGNMENT 4K
#endif

#define DEFINE_LDSYM_NULL(name) \
	PROVIDE(__ldsym__##name = .); \
	QUAD(0); \

#define DEFINE_LDSYM(name) \
	PROVIDE(__ldsym__##name = .); \
	QUAD(__##name); \

#define DEFINE_LDSYM_CUSTOM(name, value) \
	PROVIDE(__ldsym__##name = .); \
	QUAD(value); \

#define DEFINE_SECTION_LDSYMS(name)\
	DEFINE_LDSYM(kernel_##name##_vma) \
	DEFINE_LDSYM(kernel_##name##_lma) \
	DEFINE_LDSYM(kernel_##name##_size) \

#define DEFINE_SECTION_LDSYMS_NULL(name)\
	DEFINE_LDSYM_NULL(kernel_##name##_vma) \
	DEFINE_LDSYM_NULL(kernel_##name##_lma) \
	DEFINE_LDSYM_NULL(kernel_##name##_size) \

#define DEFINE_SECTION_START(name, lma, vma) \
	. = ALIGN(SECTION_ALIGNMENT); \
	SECTION_VMA(name) = vma; \
	SECTION_LMA(name) = lma; \

#define DEFINE_SECTION_SIZE(name, size) \
	. = ALIGN(SECTION_ALIGNMENT); \
	SECTION_SIZE(name) = size;

#define DEFINE_SECTION(name, ...) \
	. = ALIGN(SECTION_ALIGNMENT); \
	DEFINE_SECTION_START(name, . - __tot_hole_size, .) \
	__VA_ARGS__ \
	DEFINE_SECTION_SIZE(name, . - SECTION_VMA(name))


#define SET_HOLE(size) \
	PROVIDE(__tot_hole_size = size);

#define ADD_HOLE(delta_size) \
	__tot_hole_size += delta_size;

#define SECTION_VMA(name) __kernel_##name##_vma
#define SECTION_LMA(name) __kernel_##name##_lma
#define SECTION_SIZE(name) __kernel_##name##_size


#endif
