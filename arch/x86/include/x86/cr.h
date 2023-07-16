#ifndef _x86__CR_H__
#define _x86__CR_H__

#include <compiler_attributes.h>
#include <kstd/enum.h>

namespace x86 {

struct CR0_Flag {
	enum : unsigned long {
		PE = 1 << 0,
		MP = 1 << 1,
		EM = 1 << 2,
		TS = 1 << 3,
		ET = 1 << 4,
		NE = 1 << 5,
		WP = 1 << 16,
		AM = 1 << 18,
		NW = 1 << 29,
		CD = 1 << 30,
		PG = unsigned(1) << 31,
	};
};

struct CR4_Flag {
	enum : unsigned long {
		VME 		= 1 << 0,
		PVI 		= 1 << 1,
		TSD 		= 1 << 2,
		DE  		= 1 << 3,
		PSE 		= 1 << 4,
		PAE 		= 1 << 5,
		MCE 		= 1 << 6,
		PGE 		= 1 << 7,
		PCE 		= 1 << 8,
		OSFXSR 		= 1 << 9,
		OSXMMEXCPT 	= 1 << 10,
		UMIP 		= 1 << 11,
		LA57 		= 1 << 12,
		VMXE 		= 1 << 13,
		SMXE 		= 1 << 14,
		FSFSBASE 	= 1 << 16,
		PCIDE 		= 1 << 17,
		OSXSAVE 	= 1 << 18,
		KL 		= 1 << 19,
		SMEP 		= 1 << 20,
		SMAP 		= 1 << 21,
		PKE 		= 1 << 22,
		CET 		= 1 << 23,
		PKS 		= 1 << 24,
		UINTR 		= 1 << 25
	};
};

struct EFER_Flag {
	enum : unsigned long {
		SCE 	= 1 << 0,
		DPE 	= 1 << 1,
		SEWBED 	= 1 << 2,
		GEWBED 	= 1 << 3,
		L2D    	= 1 << 4,
		LME    	= 1 << 8,
		LMA    	= 1 << 10,
		NXE    	= 1 << 11,
		SVME   	= 1 << 12,
		LMSLE  	= 1 << 13,
		FFXSR  	= 1 << 14,
		CE    	= 1 << 15,
		COMMIT	= 1 << 17,
		INTWB  	= 1 << 18,
		UAIE   	= 1 << 20,
		AIBRSE 	= 1 << 21,
	};
};

constexpr auto efer_msr_num = 0xC0000080;

__FORCE_INLINE unsigned long read_cr0()
{
	unsigned long val;
	asm volatile ("mov %%cr0, %0" : "=r"(val));
	return val;
}

__FORCE_INLINE void write_cr0(unsigned long val)
{
	asm volatile ("mov %0, %%cr0" :: "r"(val) : "memory");
}

__FORCE_INLINE unsigned long read_cr3()
{
	unsigned long val;
	asm volatile ("mov %%cr3, %0" : "=r"(val));
	return val;
}

__FORCE_INLINE void write_cr3(unsigned long val)
{
	asm volatile ("mov %0, %%cr3" :: "r"(val) : "memory");
}

__FORCE_INLINE unsigned long read_cr4()
{
	unsigned long val;
	asm volatile ("mov %%cr4, %0" : "=r"(val));
	return val;
}

__FORCE_INLINE void write_cr4(unsigned long val)
{
	asm volatile ("mov %0, %%cr4" :: "r"(val) : "memory");
}

__FORCE_INLINE unsigned long read_efer()
{
	unsigned val;
	asm volatile ( 	"mov %[msr_efer], %%ecx 	\n"
			"rdmsr"
			: "=a"(val)
			: [msr_efer] "i" (efer_msr_num)
			: "memory");
	return val;
}

__FORCE_INLINE void write_efer(unsigned long val)
{
	asm volatile ( 	"mov %[msr_efer], %%ecx 	\n"
			"mov %[val], %%eax 		\n"
			"wrmsr"
			:: [msr_efer] "i" (efer_msr_num), [val] "r" (val)
			: "memory");
}

}

#endif
