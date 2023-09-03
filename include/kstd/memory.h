#ifndef _KSTD__MEMORY_H__
#define _KSTD__MEMORY_H__

#include <stddef.h>

namespace kstd {

using Byte = unsigned char;

template<typename Ptr_T = Byte *>
struct MemoryRange_ {
	Ptr_T beg;
	Ptr_T end;
};
using MemoryRange = MemoryRange_<>;

template<typename Ptr_T = void *, typename Size_T = size_t>
struct MemorySpan_ {
	Ptr_T ptr;
	Size_T size;
};
using MemorySpan = MemorySpan_<>;

}

#endif
