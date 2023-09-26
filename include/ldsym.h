#ifndef __LDSYM_H__
#define __LDSYM_H__

#ifndef __ASSEMBLER__

#include <stdint.h>

#define DECLARE_LDSYM(name) \
extern uint64_t __ldsym__##name __attribute__((section(".ldsym")))

#else

#define DECLARE_LDSYM(name) \
.globl __ldsym__##name

#endif // __ASSEMBLER__

#ifdef __cplusplus
extern "C" {
#endif

/* Link defined symbol declarations go here. */

DECLARE_LDSYM(stack_top);
DECLARE_LDSYM(kernel_image_end_vma);
DECLARE_LDSYM(kernel_image_end_lma);
DECLARE_LDSYM(kernel_text_start_vma);
DECLARE_LDSYM(kernel_text_start_lma);
DECLARE_LDSYM(kernel_text_size);
DECLARE_LDSYM(kernel_bss_start_vma);
DECLARE_LDSYM(kernel_bss_start_lma);
DECLARE_LDSYM(kernel_bss_size);
DECLARE_LDSYM(kernel_rodata_start_vma);
DECLARE_LDSYM(kernel_rodata_start_lma);
DECLARE_LDSYM(kernel_rodata_size);
DECLARE_LDSYM(kernel_data_start_vma);
DECLARE_LDSYM(kernel_data_start_lma);
DECLARE_LDSYM(kernel_data_size);
DECLARE_LDSYM(kernel_main);

#ifdef __cplusplus
}
#endif

#endif
