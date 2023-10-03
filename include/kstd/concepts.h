#ifndef _KSTD__CONCEPTS_H__
#define _KSTD__CONCEPTS_H__

#include <kstd/type_traits.h>

namespace kstd {

template<typename T>
concept Enum = kstd::is_enum_v<T>;

template<typename T>
concept Integral = kstd::is_integral_v<T>;

}

#endif
