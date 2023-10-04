#ifndef _KSTD__IO_H__
#define _KSTD__IO_H__

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>

#include <kstd/type_traits.h>
#include <kstd/concepts.h>
#include <kstd/limits.h>
#include <kstd/new.h>

namespace kstd {

enum class StrCase {
	Lower,
	Upper,
};

enum class BoolFormat {
	NoAlpha,
	Alpha,
};

enum class IntBase {
	Dec = 0,
	Bin,
	Oct,
	Hex,
};


/* Basic I/O stream class. */
template<typename CharT>
class Basic_IOStream {
public:
	/* Set string case for printing numeric values containing letters. */
	void set_case(StrCase caseness);
	/* Set bool format */
	void set_bool_format(BoolFormat bool_format);
	/* Set the integral base (Decimal/Binary/Octal/Hexadecimal). */
	void set_base(IntBase base);
	/* Set the filling character. */
	void set_fill_char(CharT fill_char);
	/* Set minimal width of numeric values. */
	void set_width(unsigned int width);

protected:
	StrCase caseness = StrCase::Lower;
	BoolFormat bool_format = BoolFormat::NoAlpha;
	IntBase int_base = IntBase::Dec;
	CharT fill_char = 0;
	unsigned int width = 0;
};

template<typename CharT>
inline void Basic_IOStream<CharT>::set_case(StrCase caseness)
{
	this->caseness = caseness;
}

template<typename CharT>
inline void Basic_IOStream<CharT>::set_bool_format(BoolFormat bool_format)
{
	this->bool_format = bool_format;
}

template<typename CharT>
inline void Basic_IOStream<CharT>::set_base(IntBase int_base)
{
	this->int_base = int_base;
}

template<typename CharT>
inline void Basic_IOStream<CharT>::set_fill_char(CharT fill_char)
{
	this->fill_char = fill_char;
}

template<typename CharT>
inline void Basic_IOStream<CharT>::set_width(unsigned int width)
{
	this->width = width;
}


/* Some basic output stream object class. */
template<typename CharT>
class Basic_OStream : public Basic_IOStream<CharT> {
public:
	virtual void putc(const CharT c) = 0; /* Put character. */
	virtual void puts(const CharT *str) = 0; /* Put null-terminated string. */
	/* Write 'len' characters of string 'str'. */
	virtual void write(const CharT *str, size_t len) = 0;
	virtual ~Basic_OStream() = default;

	void write_bool(bool b);

	template<Integral IntegralT>
	void write_integral(IntegralT integral);

	void write_pointer(const void *ptr);

private:
	template<Integral IntegralT>
	void write_integral_dec(IntegralT integral);

	template<Integral IntegralT>
	void write_integral_bin(IntegralT integral);

	template<Integral IntegralT>
	void write_integral_oct(IntegralT integral);

	template<Integral IntegralT>
	void write_integral_hex(IntegralT integral);

	void write_integral_str(CharT *str, int len);

	using Basic_IOStream<CharT>::caseness;
	using Basic_IOStream<CharT>::bool_format;
	using Basic_IOStream<CharT>::int_base;
	using Basic_IOStream<CharT>::fill_char;
	using Basic_IOStream<CharT>::width;
};

template<typename CharT>
void Basic_OStream<CharT>::write_bool(bool b)
{
	switch (bool_format) {
	case BoolFormat::NoAlpha:
		putc(int(b) + '0');
		break;
	case BoolFormat::Alpha:
		switch (caseness) {
		case StrCase::Lower:
			puts(b ? "true" : "false");
			break;
		case StrCase::Upper:
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
	switch (int_base) {
	case IntBase::Dec:
		write_integral_dec(integral);
		break;
	case IntBase::Bin:
		write_integral_bin(integral);
		break;
	case IntBase::Oct:
		write_integral_oct(integral);
		break;
	case IntBase::Hex:
		write_integral_hex(integral);
		break;
	default:
		break;
	}
}

template<typename CharT>
void Basic_OStream<CharT>::write_pointer(const void *ptr)
{
	puts("0x");
	write_integral_hex((uintptr_t)ptr);
}

template<typename CharT> template<Integral IntegralT>
void Basic_OStream<CharT>::write_integral_dec(IntegralT integral)
{
	const bool is_neg = integral < 0;
	constexpr int buf_len = kstd::IntegralLimits<IntegralT>::digits10()
				+ is_signed_v<IntegralT>;
	CharT buffer[buf_len] {fill_char};
	CharT *buf_start = buffer + buf_len;

	using UIntegralT = typename Unsigned<IntegralT>::Type;
	UIntegralT u_integral = is_neg ? -integral : integral;

	do {
		*--buf_start = (u_integral % 10) + '0';
		u_integral /= 10;
	} while (u_integral != 0);

	if (is_neg)
		*--buf_start = '-';

	write_integral_str(buf_start, buf_len - (buf_start - buffer));
}

template<typename CharT> template<Integral IntegralT>
void Basic_OStream<CharT>::write_integral_bin(IntegralT integral)
{
	constexpr int buf_len = kstd::IntegralLimits<IntegralT>::digits2();
	CharT buffer[buf_len] {fill_char};
	CharT *buf_start = buffer + buf_len;

	do {
		*--buf_start = integral & 0x1 + '0';
		integral >>= 1;
	} while (integral != 0);

	write_integral_str(buf_start, buf_len - (buf_start - buffer));
}

template<typename CharT> template<Integral IntegralT>
void Basic_OStream<CharT>::write_integral_oct(IntegralT integral)
{
	constexpr int buf_len = kstd::IntegralLimits<IntegralT>::digits8();
	CharT buffer[buf_len] {fill_char};
	CharT *buf_start = buffer + buf_len;

	do {
		*--buf_start = (integral & 0x7) + '0';
		integral >>= 3;
	} while (integral != 0);

	write_integral_str(buf_start, buf_len - (buf_start - buffer));
}

template<typename CharT> template<Integral IntegralT>
void Basic_OStream<CharT>::write_integral_hex(IntegralT integral)
{
	constexpr int buf_len = kstd::IntegralLimits<IntegralT>::digits16();
	CharT buffer[buf_len] {fill_char};
	CharT *buf_start = buffer + buf_len;

	do {
		char digit = (integral & 0xF);
		CharT letter_a = caseness == StrCase::Upper ? 'A' : 'a';
		*--buf_start = digit + (digit >= 10 ? (letter_a - 10) : '0');
		integral >>= 4;
	} while (integral != 0);

	write_integral_str(buf_start, buf_len - (buf_start - buffer));
}

template<typename CharT>
void Basic_OStream<CharT>::write_integral_str(CharT *str, int len)
{
	// fill with leading fill_char characters and write the string integer
	for (int i = len; i < width; ++i)
		putc(fill_char);
	write(str, len);
}

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

template<typename CharT>
inline Basic_OStream<CharT>& operator<<(Basic_OStream<CharT>& ostream, const void *ptr)
{
	ostream.write_pointer(ptr);
	return ostream;
}


template<typename CharT>
void lowercase(Basic_IOStream<CharT>& stream)
{
	stream.set_case(StrCase::Lower);
}

template<typename CharT>
void uppercase(Basic_IOStream<CharT>& stream)
{
	stream.set_case(StrCase::Upper);
}

template<typename CharT>
void noboolalpha(Basic_IOStream<CharT>& stream)
{
	stream.set_bool_format(BoolFormat::NoAlpha);
}

template<typename CharT>
void boolalpha(Basic_IOStream<CharT>& stream)
{
	stream.set_bool_format(BoolFormat::Alpha);
}

template<typename CharT>
void dec(Basic_IOStream<CharT>& stream)
{
	stream.set_base(IntBase::Dec);
}

template<typename CharT>
void bin(Basic_IOStream<CharT>& stream)
{
	stream.set_base(IntBase::Bin);
}

template<typename CharT>
void oct(Basic_IOStream<CharT>& stream)
{
	stream.set_base(IntBase::Oct);
}

template<typename CharT>
void hex(Basic_IOStream<CharT>& stream)
{
	stream.set_base(IntBase::Hex);
}

template<typename CharT>
Basic_OStream<CharT>& operator<<(Basic_OStream<CharT>& stream,
		void (*modifier)(Basic_IOStream<CharT>&))
{
	modifier(stream);
	return stream;
}


template<typename CharT>
struct SetFillParams {
	CharT fill_char;
};

template<typename CharT>
SetFillParams<CharT> setfill(CharT fill_char)
{
	return {fill_char};
}

template<typename CharT>
Basic_OStream<CharT>& operator<<(Basic_OStream<CharT>& stream,
		SetFillParams<CharT> set_fill_params)
{
	stream.set_fill_char(set_fill_params.fill_char);
	return stream;
}


struct SetWidthParams {
	unsigned int width;
};

inline SetWidthParams setw(unsigned int width)
{
	return {width};
}

template<typename CharT>
Basic_OStream<CharT>& operator<<(Basic_OStream<CharT>& stream,
		SetWidthParams set_width_params)
{
	stream.set_width(set_width_params.width);
	return stream;
}


using OStream = Basic_OStream<char>;

}

#endif
