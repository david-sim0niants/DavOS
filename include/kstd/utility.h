#ifndef _KSTD__UTILITY_H__
#define _KSTD__UTILITY_H__

#include "type_traits.h"

namespace kstd {

template<typename T>
constexpr typename RemoveReference<T>::Type&& move(T&& t)
{
	return static_cast<typename RemoveReference<T>::Type&&>(t);
}

template<typename T>
constexpr T&& forward(typename RemoveReference<T>::Type& t)
{
	return static_cast<T&&>(t);
}

template<typename T>
constexpr T&& forward(typename RemoveReference<T>::Type&& t)
{
	return static_cast<T&&>(t);
}

}

#endif
