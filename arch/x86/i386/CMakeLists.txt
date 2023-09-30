# Build kernel entry for i386 mode.
set(TARGET_NAME kernel_x86_i386_entry)

set(SRC_FILES entry.cc gdt.cc ../cpuid.cc ../page_map.cc)

add_library(${TARGET_NAME} STATIC ${SRC_FILES})
if (${CONFIG_ARCH} STREQUAL x86_64)
	target_compile_options(${TARGET_NAME} PRIVATE -m32 -mno-sse)
endif ()
target_include_directories(${TARGET_NAME} PUBLIC
	${x86_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS})
target_link_libraries(${TARGET_NAME} PRIVATE arch_x86_utils_vga klibc)
