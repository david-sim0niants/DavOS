# Build multiboot2 compliant kernel entry.
set(TARGET_NAME kernel_x86_mb2_entry)

add_library(${TARGET_NAME} INTERFACE)

target_sources(${TARGET_NAME} INTERFACE entry.S entry.cc multiboot_info.cc)

if (${CONFIG_ARCH} STREQUAL x86_64)
	target_compile_options(${TARGET_NAME} INTERFACE -m32 -mno-sse)
endif ()

set(INC_DIRS ${ROOT_INCLUDE_DIRS} ${KERNEL_INCLUDE_DIR}
	${x86_INCLUDE_DIRS} ${ARCH_INCLUDE_DIR})
target_include_directories(${TARGET_NAME} INTERFACE ${INC_DIRS})
target_link_libraries(${TARGET_NAME} INTERFACE klibc)
