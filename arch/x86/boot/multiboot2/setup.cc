#include <x86/boot/multiboot2/multiboot_info.h>


extern "C" int x86_setup(const void *mb_info_tags_struct)
{
	x86::MultibootInfo info;
	x86::read_multiboot_info(mb_info_tags_struct, info);
}
