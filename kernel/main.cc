#include <kernel/runtime.h>
#include <kernel/kout.h>

#include <arch/boot/setup.h>

namespace kernel {

extern "C" __attribute__((section(".text")))
void main(arch::BootInfo *boot_info)
{
	static_init();
	arch::setup(boot_info);
	kout << "\033c\033[3m" << "Successfully entered the main() entry.\n";
	asm volatile ("hlt");
}

}
