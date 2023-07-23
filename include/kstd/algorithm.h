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
}

#endif