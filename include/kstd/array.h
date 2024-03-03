#ifndef _KSTD__ARRAY_H__
#define _KSTD__ARRAY_H__

#include <stddef.h>
#include <string.h>

namespace kstd {

template<typename T, size_t N>
struct Array {
	T arr[N];

	T *data();
	const T *data() const;
	constexpr size_t size() const;

	constexpr T& operator[](size_t i);
	constexpr const T& operator[](size_t i) const;
};

template <typename T, typename... Args>
Array(T, Args...) -> Array<T, sizeof...(Args) + 1>;


template<typename T, size_t N>
T *Array<T, N>::data()
{
	return arr;
}

template<typename T, size_t N>
const T *Array<T, N>::data() const
{
	return arr;
}

template<typename T, size_t N>
constexpr size_t Array<T, N>::size() const
{
	return N;
}

template<typename T, size_t N>
constexpr T& Array<T, N>::operator[](size_t i)
{
	return arr[i];
}

template<typename T, size_t N>
constexpr const T& Array<T, N>::operator[](size_t i) const
{
	return arr[i];
}

}

#endif
