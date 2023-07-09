#include <stdint.h>
#include "gdt.h"

namespace x86 {

/* The global descriptor table used to switch to the long mode. */
static GDT_Entry gdt[] = {
	{ /* null segment descriptor */ }, 
	{ /* code segment descriptor */
		.limit_low = 0xFFFF,
		.base_low = 0,
		.access = DT_ACCESS_S_BIT | DT_ACCESS_P_BIT
			| DT_ACCESS_E_BIT | DT_ACCESS_RW_BIT,
		.limit_high = 0xF,
		.flags = DT_FLAGS_G_BIT | DT_FLAGS_L_BIT,
		.base_high = 0,
	},
	{ /* data segment descriptor */
		.limit_low = 0xFFFF,
		.base_low = 0,
		.access = DT_ACCESS_S_BIT | DT_ACCESS_P_BIT | DT_ACCESS_RW_BIT,
		.limit_high = 0xF,
		.flags = DT_FLAGS_G_BIT | DT_FLAGS_DB_BIT,
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
	//
	// asm volatile (  "lgdt %[gdt_ptr] 		\n"
	// 		"mov %[ds_offset], %%ax		\n"
	// 		"mov %%ax, %%ds  		\n
	// 		"mov %%ax, %%es        		\n"
	// 		"mov %%ax, %%fs 		\n"
	// 		"mov %%ax, %%gs 		\n"
	// 		"mov %%ax, %%ss 		\n"
	// 		"jmp %[cs_offset], $.Lnext 	\n"
	// 		".Lnext: 			\n"
	// 		:: [gdt_ptr]"m"(gdt_ptr),
	// 		   [cs_offset]"i"(sizeof(GDT_Entry) * 1),// code offset
	// 		   [ds_offset]"i"(sizeof(GDT_Entry) * 2) // data offset
	// 		: "memory");
	// return;
}

}
