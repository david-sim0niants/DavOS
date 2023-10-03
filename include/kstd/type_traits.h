#ifndef _KSTD__TYPE_TRAITS_H__
#define _KSTD__TYPE_TRAITS_H__

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

template<typename T1, typename T2>
constexpr bool types_match_v = types_match<T1, T2>();


template<typename T> struct RemoveReference 	 { typedef T Type; };
template<typename T> struct RemoveReference<T&>  { typedef T Type; };
template<typename T> struct RemoveReference<T&&> { typedef T Type; };

template<class T> using UnderlyingType = __underlying_type(T);

template<class T>
constexpr bool is_enum()
{
	return __is_enum(T);
}

template<class T> constexpr bool is_enum_v = is_enum<T>();


template<typename T> constexpr bool is_integral() 	{ return false; }

template<> constexpr bool is_integral<bool>() 		{ return true; }

template<> constexpr bool is_integral<char>() 		{ return true; }

template<> constexpr bool is_integral<signed char>() 	{ return true; }

template<> constexpr bool is_integral<unsigned char>() 	{ return true; }

template<> constexpr bool is_integral<wchar_t>() 	{ return true; }

template<> constexpr bool is_integral<char16_t>() 	{ return true; }

template<> constexpr bool is_integral<char32_t>() 	{ return true; }

template<> constexpr bool is_integral<short>() 		{ return true; }

template<> constexpr bool is_integral<unsigned short>() { return true; }

template<> constexpr bool is_integral<int>() 		{ return true; }

template<> constexpr bool is_integral<unsigned int>() 	{ return true; }

template<> constexpr bool is_integral<long>() 		{ return true; }

template<> constexpr bool is_integral<unsigned long>() 	{ return true; }

template<> constexpr bool is_integral<long long>() 		{ return true; }

template<> constexpr bool is_integral<unsigned long long>() 	{ return true; }

template<typename T> constexpr bool is_integral_v = is_integral<T>();

}

#endif
