#ifndef x86__ADDRESSING_H__
#define x86__ADDRESSING_H__

#include <stdint.h>
#include <x86/config.h>

namespace x86 {

#if CONFIG_x86_PHYS_ADDR_64BIT
using PhysAddr = uint64_t;
#else
using PhysAddr = uint32_t;
#endif

#if CONFIG_ARCH_BITNESS == 64
using LineAddr = uint64_t;
#else
using LineAddr = uint32_t;
#endif

}

#endif
