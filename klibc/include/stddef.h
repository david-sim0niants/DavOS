#ifndef __STDDEF_H__
#define __STDDEF_H__

#define NULL ((void *)0)

typedef __PTRDIFF_TYPE__ 	ptrdiff_t;
typedef __SIZE_TYPE__ 		size_t;

#define offsetof(type, member) ((size_t)&(((type *)0)->member))

#endif
