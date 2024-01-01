#ifndef _KSTD__MEMORY_H__
#define _KSTD__MEMORY_H__

#include <stddef.h>
#include <stdint.h>

namespace kstd {

using Byte = unsigned char;

template<typename Ptr_T = Byte *>
struct MemoryRange_ {
	Ptr_T beg {};
	Ptr_T end {};

	constexpr MemoryRange_() = default;
	constexpr MemoryRange_(Ptr_T beg, Ptr_T end)
		: beg(beg), end(end)
	{}
};
using MemoryRange = MemoryRange_<>;

template<typename Ptr_T = void *, typename Size_T = size_t>
struct MemorySpan_ {
	Ptr_T ptr {};
	Size_T size {};

	constexpr MemorySpan_() = default;
	constexpr MemorySpan_(Ptr_T ptr, Size_T size)
		: ptr(ptr), size(size) {}
};
using MemorySpan = MemorySpan_<>;

/* Align specified value by specified alignment flooring it. */
template<typename T>
inline T align_floored(T value, unsigned int alignment)
{
	return (value >> alignment) << alignment;
}

/* Align specified value by specified alignment ceiling it.
 * Will overflow to 0 if the value is greater
 * than the max value it can be with the given alignment.*/
template<typename T>
inline T align_ceiled(T value, unsigned int alignment)
{
	T floored = align_floored(value, alignment);
	return floored + (value != floored) * (1 << alignment);
}

}

#endif
