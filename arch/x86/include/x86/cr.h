#ifndef _x86__CR_H__
#define _x86__CR_H__

#include <compiler_attributes.h>

namespace x86 {

enum CR0_Bits {
	CR0_BIT_PE = 0,
	CR0_BIT_MP = 1,
	CR0_BIT_EM = 2,
	CR0_BIT_TS = 3,
	CR0_BIT_ET = 4,
	CR0_BIT_NE = 5,
	CR0_BIT_WP = 16,
	CR0_BIT_AM = 18,
	CR0_BIT_NW = 29,
	CR0_BIT_CD = 30,
	CR0_BIT_PG = 31,
};

enum CR4_Bits {
	CR4_BIT_VME 		= 0,
	CR4_BIT_PVI 		= 1,
	CR4_BIT_TSD 		= 2,
	CR4_BIT_DE  		= 3,
	CR4_BIT_PSE 		= 4,
	CR4_BIT_PAE 		= 5,
	CR4_BIT_MCE 		= 6,
	CR4_BIT_PGE 		= 7,
	CR4_BIT_PCE 		= 8,
	CR4_BIT_OSFXSR 		= 9,
	CR4_BIT_OSXMMEXCPT 	= 10,
	CR4_BIT_UMIP 		= 11,
	CR4_BIT_LA57 		= 12,
	CR4_BIT_VMXE 		= 13,
	CR4_BIT_SMXE 		= 14,
	CR4_BIT_FSFSBASE 	= 16,
	CR4_BIT_PCIDE 		= 17,
	CR4_BIT_OSXSAVE 	= 18,
	CR4_BIT_KL 		= 19,
	CR4_BIT_SMEP 		= 20,
	CR4_BIT_SMAP 		= 21,
	CR4_BIT_PKE 		= 22,
	CR4_BIT_CET 		= 23,
	CR4_BIT_PKS 		= 24,
	CR4_BIT_UINTR 		= 25
};

enum EFER_Bits {
	EFER_BIT_SCE 		= 0,
	EFER_BIT_DPE 		= 1,
	EFER_BIT_SEWBED 	= 2,
	EFER_BIT_GEWBED 	= 3,
	EFER_BIT_L2D    	= 4,
	EFER_BIT_LME    	= 8,
 	EFER_BIT_LMA    	= 10,
 	EFER_BIT_NXE    	= 11,
 	EFER_BIT_SVME   	= 12,
 	EFER_BIT_LMSLE  	= 13,
 	EFER_BIT_FFXSR  	= 14,
 	EFER_BIT_TCE    	= 15,
 	EFER_BIT_MCOMMIT	= 17,
 	EFER_BIT_INTWB  	= 18,
 	EFER_BIT_UAIE   	= 20,
 	EFER_BIT_AIBRSE 	= 21,
};

static constexpr auto MSR_EFER = 0xC0000080;


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

__FORCE_INLINE void enable_efer()
{
	asm volatile ("mov %[MSR_EFER], %%ecx"
			:: [MSR_EFER] "i" (MSR_EFER) : "memory");
}

__FORCE_INLINE unsigned long read_efer()
{
	unsigned val;
	asm volatile ("rdmsr" : "=a"(val));
	return val;
}

__FORCE_INLINE void write_efer(unsigned long val)
{
	asm volatile ( 	"mov %[val], %%eax 	\n"
			"wrmsr" :: [val] "r" (val) : "memory");
}

}

#endif
