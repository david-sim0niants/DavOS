# Build multiboot2 compliant kernel entry.
set(TARGET_NAME kernel_x86_mb2_entry)
add_library(${TARGET_NAME} STATIC entry.S entry.cc multiboot_info.cc)

if (${CONFIG_ARCH} STREQUAL x86_64)
	target_compile_options(${TARGET_NAME} PRIVATE -m32 -mno-sse)
endif ()

set(INC_DIRS ${ROOT_INCLUDE_DIRS} ${KERNEL_INCLUDE_DIR}
	${x86_INCLUDE_DIRS} ${ARCH_INCLUDE_DIR})
target_include_directories(${TARGET_NAME} PRIVATE ${INC_DIRS})
target_link_libraries(${TARGET_NAME} PRIVATE klibc)
