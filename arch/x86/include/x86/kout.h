#ifndef _x86__KOUT_H__
#define _x86__KOUT_H__

#include <kstd/io.h>
#include <x86/utils/vga/ostream.h>

namespace x86 {

// the default output stream for now
using KernelOStream = utils::VGA_OStream;
inline KernelOStream kout;

}

#endif
