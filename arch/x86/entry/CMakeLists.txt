# Build kernel entry for i386 mode.
set(TARGET_NAME kernel_x86_i386_entry)

set(SRC_FILES i386_entry.cc gdt.cc ../cpuid.cc ../page_map.cc)

add_library(${TARGET_NAME} INTERFACE)

target_sources(${TARGET_NAME} INTERFACE ${SRC_FILES})

if (${CONFIG_ARCH} STREQUAL x86_64)
	target_compile_options(${TARGET_NAME} INTERFACE -m32 -mno-sse)
endif ()

target_include_directories(${TARGET_NAME} INTERFACE
	${x86_INCLUDE_DIRS} ${ARCH_INCLUDE_DIR} ${ROOT_INCLUDE_DIRS}
	${KERNEL_INCLUDE_DIR} ${TARGETS_INLUDE_DIR})
target_link_libraries(${TARGET_NAME} INTERFACE arch_x86_utils_vga klibc)


set(TARGET_NAME kernel_entry_x86_64)
add_library(${TARGET_NAME} INTERFACE)
target_sources(${TARGET_NAME} INTERFACE x86_64_entry.cc)
target_include_directories(${TARGET_NAME} INTERFACE
	${x86_INCLUDE_DIRS} ${ARCH_INCLUDE_DIR} ${ROOT_INCLUDE_DIRS}
	${KERNEL_INCLUDE_DIR} ${TARGETS_INLUDE_DIR})
target_link_libraries(${TARGET_NAME} INTERFACE klibc)
