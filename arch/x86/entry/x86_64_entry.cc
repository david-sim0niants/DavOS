#include <x86/entry/x86_64_entry.h>
#include <ldsym.h>

namespace x86 {

extern "C" __attribute__((section(".x86_64.text"))) void _x86_64_entry(BootInfo *boot_info)
{
	reinterpret_cast<void (*)(arch::BootInfo *)>(__ldsym__kernel_main)(boot_info);
}

}
