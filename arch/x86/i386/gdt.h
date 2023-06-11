#ifndef _x86_i386__GDT_H__
#define _x86_i386__GDT_H__

#include <compiler_attributes.h>
#include <stdint.h>

namespace x86 {

enum DT_AccessBits {
	DT_ACCESS_A_BIT 	= 0x01,
	DT_ACCESS_RW_BIT 	= 0x02,
	DT_ACCESS_DC_BIT 	= 0x04,
	DT_ACCESS_E_BIT 	= 0x08,
	DT_ACCESS_S_BIT 	= 0x10,
	DT_ACCESS_DPL_BITS 	= 0x20,
	DT_ACCESS_P_BIT 	= 0x40,
};

enum DT_Flags {
	DT_FLAGS_L_BIT 	= 0x2,
	DT_FLAGS_DB_BIT = 0x4,
	DT_FLAGS_G_BIT 	= 0x8,
};

struct DescriptorTable {
	uint16_t limit_0;
	uint16_t base_0;
	uint8_t base_1;
	uint8_t access;
	uint8_t limit_1 : 4;
	uint8_t flags : 4;
	uint8_t base_2;
} __attribute__((packed));

struct GDT {
	DescriptorTable null {};
	DescriptorTable code;
	DescriptorTable data;
} __attribute__((packed));

}

#endif
