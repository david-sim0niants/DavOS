#ifndef _KSTD__OVERFLOW_H__
#define _KSTD__OVERFLOW_H__

namespace kstd {

template<typename A, typename B, typename C>
inline bool add_overflow(A a, B b, C& c)
{
	return __builtin_add_overflow(a, b, &c);
}

template<typename A_R, typename B>
inline bool add_overflow(A_R& a_and_result, B b)
{
	return __builtin_add_overflow(a_and_result, b, &a_and_result);
}

template<typename A, typename B>
inline bool add_will_overflow(A a, B b)
{
	return __builtin_add_overflow_p(a, b, (__typeof__((a) + (b))) 0);
}

}

#endif
