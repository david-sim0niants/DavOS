#ifndef _x86_i386__ENTRY_H__
#define _x86_i386__ENTRY_H__

#ifdef __ASSEMBLER__

.globl _x86_i386_start

#else

#include <x86/boot/setup.h>

namespace x86 {

extern "C" void _x86_i386_start(x86::BootInfo *boot_info);

}

#endif

#endif
