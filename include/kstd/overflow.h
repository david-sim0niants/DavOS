#ifndef KSTD__OVERFLOW_H__
#define KSTD__OVERFLOW_H__

namespace kstd {

template<typename A, typename B, typename C>
inline bool add_overflow(A a, B b, C &c)
{
	return __builtin_add_overflow(a, b, &c);
}

template<typename A_R, typename B>
inline bool add_overflow(A_R &a_and_result, B b)
{
	return __builtin_add_overflow(a_and_result, b, &a_and_result);
}

}

#endif
