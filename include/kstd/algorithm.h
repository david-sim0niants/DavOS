#ifndef _KSTD__ALGORITHM_H__
#define _KSTD__ALGORITHM_H__

namespace kstd {

template<typename T>
constexpr inline const T& max(const T& a, const T& b)
{
	return (a > b) ? a : b;
}

template<typename T>
constexpr inline const T& min(const T& a, const T& b)
{
	return (a < b) ? a : b;
}

template<typename T>
constexpr inline const T& clamp(const T& v, const T& low, const T& high)
{
	return (v < low) ? low : (v > high ? high : v);
}

template<typename I, typename O>
O copy(I i_beg, I i_end, O o_beg)
{
	for (; i_beg != i_end; (void)++i_beg, (void)++o_beg)
		*o_beg = *i_beg;
	return o_beg;
}

}

#endif
