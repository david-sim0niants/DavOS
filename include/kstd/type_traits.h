#ifndef KSTD__TYPE_TRAITS_H__
#define KSTD__TYPE_TRAITS_H__

namespace kstd {

template<typename T1, typename T2>
class TypesMatch {
public:
	static constexpr bool value = false;
};

template<typename T>
class TypesMatch<T, T> {
public:
	static constexpr bool value = true;
};

template<typename T1, typename T2>
constexpr bool types_match()
{
	return TypesMatch<T1, T2>::value;
}

}

#endif
