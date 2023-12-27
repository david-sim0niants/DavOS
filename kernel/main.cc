#include <arch/boot_info.h>


extern "C" __attribute__((section(".text")))
void main(arch::BootInfo *boot_info)
{
	asm volatile ("hlt");
}
