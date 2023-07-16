#include <stdint.h>
#include "gdt.h"

namespace x86 {

/* The global descriptor table used to switch to the long mode. */
static GDT_Entry gdt[] = {
	{ /* null segment descriptor */ }, 
	{ /* code segment descriptor */
		.limit_low = 0xFFFF,
		.base_low = 0,
		.access = GDT_AccessFlags::System | GDT_AccessFlags::Executable
			| GDT_AccessFlags::Present | GDT_AccessFlags::ReadWrite,
		.limit_high = 0xF,
		.flags = GDT_Flags::Granularity | GDT_Flags::LongMode,
		.base_high = 0,
	},
	{ /* data segment descriptor */
		.limit_low = 0xFFFF,
		.base_low = 0,
		.access = GDT_AccessFlags::System
			| GDT_AccessFlags::Present | GDT_AccessFlags::ReadWrite,
		.limit_high = 0xF,
		.flags = GDT_Flags::Granularity | GDT_Flags::SizeFlag,
		.base_high = 0,
	},
};

struct {
	uint16_t size;
	uint64_t addr;
} __attribute__((packed)) gdt_ptr;

extern "C" void *setup_gdt()
{
	gdt_ptr = {
		.size = sizeof(gdt) - 1,
		.addr = reinterpret_cast<unsigned long>(&gdt[0])
	};
	return &gdt_ptr;
}

}
