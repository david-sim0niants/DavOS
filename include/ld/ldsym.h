#ifndef _LD__LDSYM_H__
#define _LD__LDSYM_H__

#define DEFINE_LDSYM(name, value) \
	PROVIDE(__ldsym__##name = .); \
	QUAD(value); \

#endif
