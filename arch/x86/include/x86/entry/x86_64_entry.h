#ifndef _x86_ENTRY__x86_64_ENTRY_H__
#define _x86_ENTRY__x86_64_ENTRY_H__

#include <x86/boot/setup.h>

namespace x86 {

extern "C" void _86_64_entry(BootInfo *boot_info);

}

#endif
