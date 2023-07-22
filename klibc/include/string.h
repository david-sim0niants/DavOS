#ifndef __STRING_H__
#define __STRING_H__

#define memcpy(__dst, __src, __size) __builtin_memcpy(__dst, __src, __size)
#define memset(__dst, __val, __size) __builtin_memset(__dst, __val, __size)

#endif
