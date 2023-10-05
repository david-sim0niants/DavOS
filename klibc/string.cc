#include <stddef.h>

extern "C" {

void *memcpy(void *dst, const void *src, size_t cnt)
{
	const char *c_src = (const char *)src;
	char *c_dst = (char *)dst;

	while (cnt--)
		*c_dst++ = *c_src++;

	return dst;
}

void *memmove(void *dst, const void *src, size_t cnt)
{
	char *c_dst = (char *)dst;
	const char *c_src = (const char *)src;

	if (c_dst <= c_src) {
		while (cnt--)
			*c_dst++ = *c_src++;
	} else {
		c_dst += cnt;
		c_src += cnt;
		while (cnt--)
			*--c_dst = *--c_src;
	}

	return dst;
}

void *memset(void *dst, const int val, size_t cnt)
{
	char *c_dst = (char *)dst;
	while (cnt--)
		*c_dst++ = val;
	return dst;
}

}
