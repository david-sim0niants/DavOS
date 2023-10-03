#ifndef _KSTD__NEW_H__
#define _KSTD__NEW_H__

#include <stddef.h>

inline void *operator new(size_t size, void *ptr)
{
	return ptr;
}

inline void operator delete(void *ptr) noexcept
{
	return;
}

inline void operator delete[](void *ptr) noexcept
{
	return;
}

inline void operator delete(void *ptr, size_t size) noexcept
{
	return;
}

inline void operator delete[](void *ptr, size_t size) noexcept
{
	return;
}


#endif
