#include <kernel/runtime.h>

#include <kernel_image.h>

#include <stddef.h>


extern "C" void* __dso_handle;
void *__dso_handle = nullptr;

extern "C" void __cxa_atexit()
{
	// do nothing for now
}

namespace kernel {

void static_init()
{
	auto init_array = reinterpret_cast<void (* const*)(void)>(kernel_image::init_array_section.vma_start);
	size_t init_array_size = kernel_image::init_array_section.size / sizeof(void *);
	for (int i = 0; i < init_array_size; ++i)
		init_array[i]();
}

}
