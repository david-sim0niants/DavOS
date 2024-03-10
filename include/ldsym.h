#ifndef __LDSYM_H__
#define __LDSYM_H__

#include <stdint.h>

#define DECLARE_LDSYM(name) \
extern uint64_t __ldsym__##name __attribute__((section(".ldsym"))); \

#define DECLARE_SEGMENT_LDSYMS(name) \
DECLARE_LDSYM(kernel_segment__##name##_vma) \
DECLARE_LDSYM(kernel_segment__##name##_lma) \
DECLARE_LDSYM(kernel_segment__##name##_size) \

#ifdef __cplusplus
extern "C" {
#endif

/* Link defined symbol declarations go here. */

DECLARE_LDSYM(kernel_stack_top);
DECLARE_LDSYM(kernel_image_start_vma);
DECLARE_LDSYM(kernel_image_start_lma);
DECLARE_LDSYM(kernel_image_end_vma);
DECLARE_LDSYM(kernel_image_end_lma);

DECLARE_SEGMENT_LDSYMS(stack);

DECLARE_SEGMENT_LDSYMS(text)
DECLARE_SEGMENT_LDSYMS(rodata)
DECLARE_SEGMENT_LDSYMS(data)

DECLARE_LDSYM(kernel_main);

#ifdef __cplusplus
}
#endif

#endif
