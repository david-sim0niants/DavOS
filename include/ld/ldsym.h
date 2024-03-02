#ifndef _LD__LDSYM_H__
#define _LD__LDSYM_H__

#ifndef SECTION_ALIGNMENT
#define SECTION_ALIGNMENT 4K
#endif

#define DEFINE_LDSYM(name, value) \
	PROVIDE(__ldsym__##name = .); \
	QUAD(value); \

#define DEFINE_SECTION_START(name, lma, vma) \
	. = ALIGN(SECTION_ALIGNMENT); \
	SECTION_START_VMA(name) = vma; \
	SECTION_START_LMA(name) = lma; \

#define DEFINE_SECTION_SIZE(name, size) \
	. = ALIGN(SECTION_ALIGNMENT); \
	SECTION_SIZE(name) = size;

#define DEFINE_SECTION(name, ...) \
	. = ALIGN(SECTION_ALIGNMENT); \
	DEFINE_SECTION_START(name, . - __TOT_HOLE_SIZE, .) \
	__VA_ARGS__ \
	DEFINE_SECTION_SIZE(name, . - SECTION_START_VMA(name))

#define SET_HOLE(size) \
	PROVIDE(__TOT_HOLE_SIZE = size);
#define ADD_HOLE(delta_size) \
	__TOT_HOLE_SIZE += delta_size;

#define SECTION_START_VMA(name) __KERNEL_##name##_START_VMA
#define SECTION_START_LMA(name) __KERNEL_##name##_START_LMA
#define SECTION_SIZE(name) __KERNEL_##name##_SIZE


#endif
