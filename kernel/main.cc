#include <arch/boot/setup.h>


extern "C" __attribute__((section(".text")))
void main(arch::BootInfo *boot_info)
{
	arch::setup(boot_info);
	asm volatile ("hlt");
}
