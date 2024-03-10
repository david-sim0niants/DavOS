#ifndef _x86__LDSYM_H__
#define _x86__LDSYM_H__

#include <ldsym.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Link defined symbol declarations go here. */

#if CONFIG_ARCH == ARCH_x86_64
	DECLARE_SEGMENT_LDSYMS(x86_64_text);
	DECLARE_SEGMENT_LDSYMS(x86_64_rodata);
	DECLARE_SEGMENT_LDSYMS(x86_64_data);

	DECLARE_LDSYM(kernel_x86_64_entry);
#endif

#ifdef __cplusplus
}
#endif


#endif
