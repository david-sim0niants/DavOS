#ifndef _LD__SECTION_H__
#define _LD__SECTION_H__

#define DEFINE_SECTION_START(name, lma, vma) \
	SECTION_VMA(name) = vma; \
	SECTION_LMA(name) = lma; \

#define DEFINE_SECTION_SIZE(name, size) \
	SECTION_SIZE(name) = size;

#define DEFINE_SECTION(name, ...) \
	DEFINE_SECTION_START(name, . - __tot_hole_size, .) \
	__VA_ARGS__ \
	DEFINE_SECTION_SIZE(name, . - SECTION_VMA(name))

#define SECTION_VMA(name) __kernel_section__##name##_vma
#define SECTION_LMA(name) __kernel_section__##name##_lma
#define SECTION_SIZE(name) __kernel_section__##name##_size

#endif
