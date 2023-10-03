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

template<> constexpr bool is_integral<char8_t>() 	{ return true; }

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


template<typename T> constexpr bool is_unsigned()
{
	static_assert(is_integral_v<T>, "Sign check is only for integral types.");
	return false;
}

template<> constexpr bool is_unsigned<bool>() 		{ return true; }

template<> constexpr bool is_unsigned<unsigned char>() 	{ return true; }

template<> constexpr bool is_unsigned<unsigned short>() { return true; }

template<> constexpr bool is_unsigned<unsigned int>() 	{ return true; }

template<> constexpr bool is_unsigned<unsigned long>() 	{ return true; }

template<> constexpr bool is_unsigned<unsigned long long>() 	{ return true; }

template<typename T> constexpr bool is_unsigned_v = is_unsigned<T>();

template<typename T> constexpr bool is_signed() 	{ return !is_unsigned<T>(); }

template<typename T> constexpr bool is_signed_v = is_signed<T>();


template<typename T>
struct Unsigned {
	static_assert(is_integral_v<T>, "Getting unsigned version of a type is only for integral types.");
	static_assert(types_match_v<char8_t, T>,  "char8_t not supported.");
	static_assert(types_match_v<char16_t, T>, "char16_t not supported.");
	static_assert(types_match_v<char32_t, T>, "char32_t not supported.");
	static_assert(types_match_v<wchar_t, T>,  "wchar_t not supported.");
	using Type = T;
};

template<> struct Unsigned<char>
{
	using Type = unsigned char;
};

template<> struct Unsigned<signed char>
{
	using Type = unsigned char;
};

template<> struct Unsigned<short>
{
	using Type = unsigned short;
};

template<> struct Unsigned<int>
{
	using Type = unsigned int;
};

template<> struct Unsigned<long>
{
	using Type = unsigned long;
};

template<> struct Unsigned<long long>
{
	using Type = unsigned long long;
};


template<typename T>
struct Signed {
	static_assert(is_integral_v<T>, "Getting signed version of a type is only for integral types.");
	using Type = T;
};

template<> struct Signed<unsigned char> {
	using Type = signed char;
};

template<> struct Signed<unsigned short> {
	using Type = short;
};

template<> struct Signed<unsigned int> {
	using Type = int;
};

template<> struct Signed<unsigned long> {
	using Type = long;
};

template<> struct Signed<unsigned long long> {
	using Type = long long;
};

}

#endif
