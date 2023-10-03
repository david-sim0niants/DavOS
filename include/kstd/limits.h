#ifndef _KSTD__LIMITS_H__
#define _KSTD__LIMITS_H__

#include <kstd/concepts.h>

namespace kstd {

constexpr auto char_bits = 8;
constexpr double log10_2 = 0.30102999566;

template<Integral IntegralT>
struct IntegralLimits {
	static constexpr int bits();
	static constexpr int digits2();
	static constexpr int digits8();
	static constexpr int digits10();
	static constexpr int digits16();
};

template<Integral IntegralT>
constexpr int IntegralLimits<IntegralT>::bits()
{
	return sizeof(IntegralT) * char_bits;
}

template<Integral IntegralT>
constexpr int IntegralLimits<IntegralT>::digits2()
{
	return bits();
}

template<Integral IntegralT>
constexpr int IntegralLimits<IntegralT>::digits8()
{
	return bits() / 3 + 1;
}

template<Integral IntegralT>
constexpr int IntegralLimits<IntegralT>::digits10()
{
	return int((bits() - is_signed_v<IntegralT>) * log10_2 + 1);
}

template<Integral IntegralT>
constexpr int IntegralLimits<IntegralT>::digits16()
{
	return bits() / 4;
}

}

#endif
