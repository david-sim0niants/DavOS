#ifndef __STRING_H__
#define __STRING_H__

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void *memcpy(void *dst, const void *src, size_t cnt);
void *memmove(void *dst, const void *src, size_t cnt);
void *memset(void *dst, const int val, size_t cnt);

size_t strlen(const char *str);
size_t strnlen(const char *str, size_t n);
char *strcpy(char *dst, const char *src);
char *strncpy(char *dst, const char *src, size_t n);

int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);

#ifdef __cplusplus
}
#endif

#define memcpy(__dst, __src, __cnt) __builtin_memcpy(__dst, __src, __cnt)
#define memset(__dst, __val, __cnt) __builtin_memset(__dst, __val, __cnt)
#define memmove(__dst, __src, __cnt) __builtin_memmove(__dst, __src, __cnt)

#define strlen(__str) __builtin_strlen(__str)
#define strnlen(__str) __builtin_strnlen(__str)
#define strcpy(__dst, __src) __builtin_strcpy(__dst, __src)
#define strncpy(__dst, __src, __n) __builtin_strncpy(__dst, __src, __n)

#define strcmp(__s1, __s2) __builtin_strcmp(__s1, __s2)
#define strncmp(__s1, __s2, __n) __builtin_strncmp(__s1, __s2, __n)

#endif
