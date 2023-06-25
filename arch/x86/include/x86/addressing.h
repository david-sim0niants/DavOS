#ifndef _x86__ADDRESSING_H__
#define _x86__ADDRESSING_H__

#include <stdint.h>
#include <x86/config.h>

namespace x86 {

#if CONFIG_x86_PHYS_ADDR_64BIT
using PhysAddr = uint64_t;
using PhysSize = uint64_t;
using PhysPageN = uint64_t;
#else
using PhysAddr = uint32_t;
using PhysSize = uint32_t;
using PhysPageN = uint32_t;
#endif

#if CONFIG_ARCH_BITNESS == 64
using LineAddr = uint64_t;
using LineSize = uint64_t;
using LinePageN = uint64_t;
#else
using LineAddr = uint32_t;
using LineSize = uint32_t;
using LinePageN = uint32_t;
#endif

}

#endif
