#ifndef _x86_i386__GDT_H__
#define _x86_i386__GDT_H__

#include <stdint.h>

#include <kstd/enum.h>

namespace x86 {

enum class GDT_AccessFlags : uint8_t {
	Accessed 	= 0x01,
	ReadWrite 	= 0x02,
	DC 		= 0x04,
	Executable 	= 0x08,
	System 		= 0x10,
	DPL_BIT_1 	= 0x20,
	DPL_BIT_2 	= 0x40,
	Present 	= 0x80,
};
KSTD_DEFINE_ENUM_LOGIC_BITWISE_OPERATORS(GDT_AccessFlags);


enum class GDT_Flags : uint8_t {
	LongMode 	= 0x2,
	SizeFlag 	= 0x4,
	Granularity 	= 0x8,
};
KSTD_DEFINE_ENUM_LOGIC_BITWISE_OPERATORS(GDT_Flags);


struct GDT_Entry {
	uint16_t limit_low;
	uint32_t base_low : 24;
	GDT_AccessFlags access;
	uint8_t limit_high : 4;
	GDT_Flags flags : 4;
	uint8_t base_high;
} __attribute__((packed));


struct GDT_Ptr {
	uint16_t size;
	uint64_t addr;
} __attribute__((packed));


extern GDT_Ptr gdt_ptr;


inline void load_gdt()
{
	asm volatile("lgdt (%[gdt_ptr])" :: [gdt_ptr]"r"(&gdt_ptr));
}

}

#endif
