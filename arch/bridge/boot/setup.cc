#include <arch/boot/setup.h>


namespace arch {

static BootInfo *boot_info;

void setup(BootInfo *boot_info)
{
	arch::boot_info = boot_info;
}

BootInfo *get_boot_info()
{
	return boot_info;
}

}
