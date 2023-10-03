#ifndef _KSTD__IO_H__
#define _KSTD__IO_H__

#include <stddef.h>
#include <kstd/type_traits.h>
#include <kstd/concepts.h>

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
};

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

}

#endif
