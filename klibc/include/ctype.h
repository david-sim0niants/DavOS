#ifndef __CTYPE_H__
#define __CTYPE_H__

#ifdef __cplusplus
extern "C" {
#endif

inline bool isdigit(int c)
{
	return '0' <= c && c <= '9';
}

inline bool isspace(int c)
{
	switch (c) {
	case ' ':
	case '\n':
	case '\r':
	case '\t':
	case '\v':
	case '\f':
		return true;
	default:
		return false;
	}
}

#ifdef __cplusplus
}
#endif

#endif
