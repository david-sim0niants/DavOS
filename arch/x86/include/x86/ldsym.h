#ifndef _x86__LDSYM_H__
#define _x86__LDSYM_H__

#include <ldsym.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Link defined symbol declarations go here. */

DECLARE_LDSYM(kernel_i386_text_start_vma);
DECLARE_LDSYM(kernel_i386_text_start_lma);
DECLARE_LDSYM(kernel_i386_text_size);
DECLARE_LDSYM(kernel_i386_rodata_start_vma);
DECLARE_LDSYM(kernel_i386_rodata_start_lma);
DECLARE_LDSYM(kernel_i386_rodata_size);
DECLARE_LDSYM(kernel_i386_data_start_vma);
DECLARE_LDSYM(kernel_i386_data_start_lma);
DECLARE_LDSYM(kernel_i386_data_size);
DECLARE_LDSYM(kernel_stack_start_vma);
DECLARE_LDSYM(kernel_stack_start_lma);
DECLARE_LDSYM(kernel_stack_size);

#ifdef __cplusplus
}
#endif


#endif
