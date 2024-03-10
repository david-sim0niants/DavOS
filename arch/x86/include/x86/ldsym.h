#ifndef _x86__LDSYM_H__
#define _x86__LDSYM_H__

#include <ldsym.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Link defined symbol declarations go here. */

DECLARE_SECTION_LDSYMS(i386_text);
DECLARE_SECTION_LDSYMS(i386_rodata);
DECLARE_SECTION_LDSYMS(i386_data);

#if CONFIG_ARCH == ARCH_x86_64
	DECLARE_SECTION_LDSYMS(x86_64_text);
	DECLARE_SECTION_LDSYMS(x86_64_rodata);
	DECLARE_SECTION_LDSYMS(x86_64_data);

	DECLARE_LDSYM(kernel_x86_64_entry);
#endif

DECLARE_SECTION_LDSYMS(stack);

#ifdef __cplusplus
}
#endif


#endif
