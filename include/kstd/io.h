#ifndef _KSTD__IO_H__
#define _KSTD__IO_H__

#include <stddef.h>
#include <stdlib.h>
#include <kstd/type_traits.h>
#include <kstd/concepts.h>
#include <kstd/limits.h>
#include <kstd/new.h>

namespace kstd {

/* Some basic output stream object class. */
template<typename CharT>
class Basic_OStream {
public:
	virtual void putc(const CharT c) = 0; /* Put character. */
	virtual void puts(const CharT *str) = 0; /* Put null-terminated string. */
	/* Write 'len' characters of string 'str'. */
	virtual void write(const CharT *str, size_t len) = 0;
	virtual ~Basic_OStream() = default;

	void write_bool(bool b);

	template<Integral IntegralT>
	void write_integral(IntegralT integral);

private:
	template<Integral IntegralT>
	void write_integral_dec(IntegralT integral);

	template<Integral IntegralT>
	void write_integral_bin(IntegralT integral);

	template<Integral IntegralT>
	void write_integral_oct(IntegralT integral);

	template<Integral IntegralT>
	void write_integral_hex(IntegralT integral);

	enum class CaseType { Lowercase, Uppercase };
	CaseType case_type;

	enum class BoolFormat { NoAlpha, Alpha };
	BoolFormat bool_format;

	enum class IntegralFormat {
		Decimal = 0,
		Binary,
		Octal,
		Hexadecimal,
	};
	IntegralFormat int_format;
	char int_fill = 0;
};

template<typename CharT>
void Basic_OStream<CharT>::write_bool(bool b)
{
	switch (bool_format) {
	case BoolFormat::NoAlpha:
		putc(int(b) + '0');
		break;
	case BoolFormat::Alpha:
		switch (case_type) {
		case CaseType::Lowercase:
			puts(b ? "true" : "false");
			break;
		case CaseType::Uppercase:
			puts(b ? "TRUE" : "FALSE");
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

template<typename CharT> template<Integral IntegralT>
void Basic_OStream<CharT>::write_integral(IntegralT integral)
{
	switch (int_format) {
	case IntegralFormat::Decimal:
		write_integral_dec(integral);
		break;
	case IntegralFormat::Binary:
		write_integral_bin(integral);
		break;
	case IntegralFormat::Octal:
		write_integral_oct(integral);
		break;
	case IntegralFormat::Hexadecimal:
		write_integral_hex(integral);
		break;
	default:
		break;
	}
}

template<typename CharT> template<Integral IntegralT>
void Basic_OStream<CharT>::write_integral_dec(IntegralT integral)
{
	const bool is_neg = integral < 0;
	constexpr int buf_len = kstd::IntegralLimits<IntegralT>::digits10()
				+ is_signed_v<IntegralT>;
	char buffer[buf_len] {int_fill};
	char *buf_start = buffer + buf_len;

	if (is_neg)
		integral = -integral;

	do {
		*--buf_start = integral % 10 + '0';
		integral /= 10;
	} while (integral > 0);

	if (int_fill != 0) [[unlikely]] {
		if (is_neg)
			buffer[0] = '-';
		write(buffer, buf_len);
	}
	else {
		if (is_neg)
			*--buf_start = '-';
		write(buf_start, buf_len - (buf_start - buffer));
	}
}

template<typename CharT> template<Integral IntegralT>
void Basic_OStream<CharT>::write_integral_bin(IntegralT integral)
{
	constexpr int buf_len = kstd::IntegralLimits<IntegralT>::digits2();
	char buffer[buf_len] {int_fill};
	char *buf_start = buffer + buf_len;

	do {
		*--buf_start = integral & 0x1 + '0';
		integral >>= 1;
	} while (integral > 0);

	if (int_fill != 0) [[unlikely]]
		write(buffer, buf_len);
	else
		write(buf_start, buf_len - (buf_start - buffer));
}

template<typename CharT> template<Integral IntegralT>
void Basic_OStream<CharT>::write_integral_oct(IntegralT integral)
{
	constexpr int buf_len = kstd::IntegralLimits<IntegralT>::digits8();
	char buffer[buf_len] {int_fill};
	char *buf_start = buffer + buf_len;

	do {
		*--buf_start = (integral & 0x7) + '0';
		integral >>= 3;
	} while (integral > 0);

	if (int_fill != 0) [[unlikely]]
		write(buffer, buf_len);
	else
		write(buf_start, buf_len - (buf_start - buffer));
}

template<typename CharT> template<Integral IntegralT>
void Basic_OStream<CharT>::write_integral_hex(IntegralT integral)
{
	constexpr int buf_len = kstd::IntegralLimits<IntegralT>::digits16();
	char buffer[buf_len] {int_fill};
	char *buf_start = buffer + buf_len;

	do {
		char digit = (integral & 0xF);
		char letter_a = case_type == CaseType::Uppercase ? 'A' : 'a';
		*--buf_start = digit + (digit >= 10 ? (letter_a - 10) : '0');
		integral >>= 4;
	} while (integral > 0);

	if (int_fill != 0) [[unlikely]]
		write(buffer, buf_len);
	else
		write(buf_start, buf_len - (buf_start - buffer));
}


using OStream = Basic_OStream<char>;

template<typename CharT>
inline Basic_OStream<CharT>& operator<<(Basic_OStream<CharT>& ostream, const CharT c)
{
	ostream.putc(c);
	return ostream;
}

template<typename CharT>
inline Basic_OStream<CharT>& operator<<(Basic_OStream<CharT>& ostream, const CharT *str)
{
	ostream.puts(str);
	return ostream;
}

template<typename CharT>
inline Basic_OStream<CharT>& operator<<(Basic_OStream<CharT>& ostream, bool b)
{
	ostream.write_bool(b);
	return ostream;
}

template<typename CharT, Integral IntegralT>
inline Basic_OStream<CharT>& operator<<(Basic_OStream<CharT>& ostream, IntegralT integral)
{
	ostream.write_integral(integral);
	return ostream;
}

}

#endif
