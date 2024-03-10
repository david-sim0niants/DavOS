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

#endif
