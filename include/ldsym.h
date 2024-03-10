#ifndef __LDSYM_H__
#define __LDSYM_H__

#include <stdint.h>

#define DECLARE_LDSYM(name) \
extern uint64_t __ldsym__##name __attribute__((section(".ldsym"))); \
extern void *__##name;

#define DECLARE_SECTION_LDSYMS(name) \
DECLARE_LDSYM(kernel_##name##_vma) \
DECLARE_LDSYM(kernel_##name##_lma) \
DECLARE_LDSYM(kernel_##name##_size) \

#ifdef __cplusplus
extern "C" {
#endif

/* Link defined symbol declarations go here. */

DECLARE_LDSYM(kernel_stack_top);

DECLARE_LDSYM(kernel_image_start_vma);
DECLARE_LDSYM(kernel_image_start_lma);
DECLARE_LDSYM(kernel_image_end_vma);
DECLARE_LDSYM(kernel_image_end_lma);

DECLARE_SECTION_LDSYMS(ldsym)
DECLARE_SECTION_LDSYMS(text)
DECLARE_SECTION_LDSYMS(bss)
DECLARE_SECTION_LDSYMS(rodata)
DECLARE_SECTION_LDSYMS(data)
DECLARE_SECTION_LDSYMS(init_array)

DECLARE_LDSYM(kernel_main);

#ifdef __cplusplus
}
#endif

#endif
