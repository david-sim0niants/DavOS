#ifndef _KSTD__ENUM_H__
#define _KSTD__ENUM_H__

#include <kstd/type_traits.h>

#define KSTD_DEFINE_ENUM_LOGIC_BITWISE_OPERATORS(enum_type) 		\
namespace __##enum_type##__logic_bitwise_operators { 			\
									\
using ul_type = ::kstd::UnderlyingType<enum_type>; 			\
									\
inline constexpr enum_type operator ~(enum_type e)			\
{ 									\
	return static_cast<enum_type>(~static_cast<ul_type>(e)); 	\
} 									\
									\
inline constexpr enum_type operator |(enum_type a, enum_type b) 	\
{ 									\
	return static_cast<enum_type>( 					\
		static_cast<ul_type>(a) | static_cast<ul_type>(b)); 	\
} 									\
									\
inline constexpr enum_type operator &(enum_type a, enum_type b)		\
{ 									\
	return static_cast<enum_type>( 					\
		static_cast<ul_type>(a) & static_cast<ul_type>(b)); 	\
} 									\
									\
inline constexpr enum_type operator ^(enum_type a, enum_type b) 	\
{ 									\
	return static_cast<enum_type>( 					\
		static_cast<ul_type>(a) ^ static_cast<ul_type>(b)); 	\
} 									\
									\
inline constexpr enum_type &operator |=(enum_type &a, enum_type b) 	\
{ 									\
	return a = a | b; 						\
} 									\
									\
inline constexpr enum_type &operator &=(enum_type &a, enum_type b) 	\
{ 									\
	return a = a & b; 						\
} 									\
									\
inline constexpr enum_type &operator ^=(enum_type &a, enum_type b) 	\
{ 									\
	return a = a ^ b; 						\
} 									\
									\
} 									\
using __##enum_type##__logic_bitwise_operators::operator~; 		\
using __##enum_type##__logic_bitwise_operators::operator&;		\
using __##enum_type##__logic_bitwise_operators::operator|;		\
using __##enum_type##__logic_bitwise_operators::operator^;		\
using __##enum_type##__logic_bitwise_operators::operator&=;		\
using __##enum_type##__logic_bitwise_operators::operator|=;		\
using __##enum_type##__logic_bitwise_operators::operator^=;		\

#define KSTD_DEFINE_ENUM_SHIFT_OPERATORS(enum_type) 			\
namespace __##enum_type##__shift_operators { 				\
									\
using ul_type = ::kstd::UnderlyingType<enum_type>; 			\
									\
inline constexpr enum_type operator >>(enum_type a, int b) 		\
{									\
	return static_cast<enum_type>(static_cast<ul_type>(a) >> b); 	\
} 									\
									\
inline constexpr enum_type operator <<(enum_type a, int b) 		\
{									\
	return static_cast<enum_type>(static_cast<ul_type>(a) << b); 	\
} 									\
									\
inline constexpr enum_type &operator >>=(enum_type &a, int b) 		\
{                                                                       \
	return a = a >> b;                                              \
}                                                                       \
                                                                        \
inline constexpr enum_type &operator <<=(enum_type &a, int b) 		\
{                                                                       \
	return a = a << b;                                              \
}                                                                       \
									\
} 									\
using __##enum_type##__shift_operators::operator>>;			\
using __##enum_type##__shift_operators::operator<<;			\
using __##enum_type##__shift_operators::operator>>=			\
using __##enum_type##__shift_operators::operator<<=			\


namespace kstd {

template<typename T>
concept Enum = kstd::is_enum<T>();

template<Enum E> bool test_flag(E main_flags, E test_flags)
{
	using UType = UnderlyingType<E>;
	return (static_cast<UType>(main_flags) & static_cast<UType>(test_flags))
		== static_cast<UType>(test_flags);
}

template<Enum E>
constexpr E switch_flag(E flag, bool on)
{
	return static_cast<E>(static_cast<kstd::UnderlyingType<E>>(flag) * on);
}

template<Enum E>
constexpr E to_enum(auto val)
{
	return static_cast<E>(val);
}

template<Enum E>
constexpr auto to_ut(E e)
{
	return static_cast<UnderlyingType<E>>(e);
}

}

#endif
