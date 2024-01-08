#ifndef _x86_ENTRY__i386_ENTRY_H__
#define _x86_ENTRY__i386_ENTRY_H__

#include <x86/boot/setup.h>

namespace x86 {

extern "C" void _i386_start(x86::BootInfo *boot_info);

}

#endif
