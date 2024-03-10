#include <kernel/runtime.h>

#include <stddef.h>


extern "C" void* __dso_handle;
void *__dso_handle = nullptr;

extern "C" void __cxa_atexit()
{
	// do nothing for now
}

extern "C" void *__kernel_init_array_vma, *__kernel_init_array_unaligned_size;

namespace kernel {

void static_init()
{
	void (**init_array)(void) = reinterpret_cast<void (**)(void)>(&__kernel_init_array_vma);
	size_t init_array_size = (size_t)(&__kernel_init_array_unaligned_size) / sizeof(void *);
	for (auto fn = init_array; init_array_size; --init_array_size, ++fn)
		(*fn)();
}

}
