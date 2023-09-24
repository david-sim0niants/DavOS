# Build multiboot2 compliant kernel entry.
set(TARGET_NAME kernel_x86_mb2_entry)
add_library(${TARGET_NAME} STATIC entry.S multiboot_info.cc)

if (${CONFIG_ARCH} STREQUAL x86_64)
	target_compile_options(${TARGET_NAME} PRIVATE -m32 -mno-sse)
endif ()

set(INC_DIRS ${ROOT_INCLUDE_DIRS} ${x86_INCLUDE_DIRS} ${KLIBC_INCLUDE_DIR})
target_include_directories(${TARGET_NAME} PRIVATE ${INC_DIRS})
