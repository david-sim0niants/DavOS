#ifndef __LDSYM_H__
#define __LDSYM_H__

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DECLARE_LDSYM(type, name) \
extern type __ldsym__##name __attribute__((section(".ldsym")))

/* Link defined config declarations go here. */

DECLARE_LDSYM(size_t, stack_top);
DECLARE_LDSYM(void *, kernel_image_end_vma);
DECLARE_LDSYM(void *, kernel_image_end_lma);
DECLARE_LDSYM(void *, kernel_text_start_vma);
DECLARE_LDSYM(void *, kernel_text_start_lma);
DECLARE_LDSYM(size_t, kernel_text_size);
DECLARE_LDSYM(void *, kernel_bss_start_vma);
DECLARE_LDSYM(void *, kernel_bss_start_lma);
DECLARE_LDSYM(size_t, kernel_bss_size);
DECLARE_LDSYM(void *, kernel_rodata_start_vma);
DECLARE_LDSYM(void *, kernel_rodata_start_lma);
DECLARE_LDSYM(size_t, kernel_rodata_size);
DECLARE_LDSYM(void *, kernel_data_start_vma);
DECLARE_LDSYM(void *, kernel_data_start_lma);
DECLARE_LDSYM(size_t, kernel_data_size);

#ifdef __cplusplus
}
#endif


#endif
