#ifndef __STDDEF_H__
#define __STDDEF_H__

#define NULL ((void *)0)

#if __SIZEOF_POINTER__ == 8
	typedef long 		ptrdiff_t;
	typedef long 		ssize_t;
	typedef unsigned long 	size_t;
#elif __SIZEOF_POINTER__ == 4
	typedef long 		ptrdiff_t;
	typedef long 		ssize_t;
	typedef unsigned long 	size_t;
#else
	#error "Unsupported bitness."
#endif

#define offsetof(type, member) ((size_t)&(((type *)0)->member))

#endif
