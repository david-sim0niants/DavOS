#ifndef _x86_i386__GDT_H__
#define _x86_i386__GDT_H__

#ifndef __ASSEMBLER__

#include <stdint.h>

namespace x86 {

enum GDT_AccessBits {
	DT_ACCESS_A_BIT 	= 0x01,
	DT_ACCESS_RW_BIT 	= 0x02,
	DT_ACCESS_DC_BIT 	= 0x04,
	DT_ACCESS_E_BIT 	= 0x08,
	DT_ACCESS_S_BIT 	= 0x10,
	DT_ACCESS_DPL_BIT_1 	= 0x20,
	DT_ACCESS_DPL_BIT_2 	= 0x40,
	DT_ACCESS_P_BIT 	= 0x80,
};

enum GDT_Flags {
	DT_FLAGS_L_BIT 	= 0x2,
	DT_FLAGS_DB_BIT = 0x4,
	DT_FLAGS_G_BIT 	= 0x8,
};

struct GDT_Entry {
	uint16_t limit_low;
	uint32_t base_low : 24;
	uint8_t access;
	uint8_t limit_high : 4;
	uint8_t flags : 4;
	uint8_t base_high;
} __attribute__((packed));

}

extern "C" void *setup_gdt();

#else

.section .text
.type setup_gdt, @function
.globl setup_gdt

#endif

#endif
