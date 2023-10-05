#ifndef __STRING_H__
#define __STRING_H__

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void *memcpy(void *dst, const void *src, size_t cnt);
extern void *memmove(void *dst, const void *src, size_t cnt);
extern void *memset(void *dst, const int val, size_t cnt);

#ifdef __cplusplus
}
#endif

#define memcpy(__dst, __src, __cnt) __builtin_memcpy(__dst, __src, __cnt)
#define memset(__dst, __val, __cnt) __builtin_memset(__dst, __val, __cnt)
#define memmove(__dst, __src, __cnt) __builtin_memmove(__dst, __src, __cnt)

#endif
