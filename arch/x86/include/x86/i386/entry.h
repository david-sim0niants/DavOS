#ifndef _x86_i386__ENTRY_H__
#define _x86_i386__ENTRY_H__

#ifdef __ASSEMBLER__

.globl _x86_i386_start

#else

extern "C" void _x86_i386_start();

#endif

#endif
