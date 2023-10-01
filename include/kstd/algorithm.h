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

}

#endif
