set(CONFIG_ARCH_x86_KERNEL_START 0x200000 CACHE STRING
    "Specifies the kernel entry point where i386 specific code will jump onto.")

# Include necessary arch-specific include directories.
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/generated/include)

# Configure arch/config.h.in file.
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/arch/config.h.in
	${CMAKE_CURRENT_BINARY_DIR}/generated/include/arch/config.h)


add_subdirectory(i386)


# Build x86 architecture specific library.
set(TARGET_NAME kernel_arch_x86)

add_library(${TARGET_NAME} STATIC null.c)
set_target_properties(${TARGET_NAME} PROPERTIES PREFIX "" SUFFIX ".elf")


# Build the uncompressed kernel image.
set(TARGET_NAME kernel)
set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld)

# Choose the target file format of the image.
if (${CONFIG_ARCH} STREQUAL x86_64)
	set(TARGET_FORMAT elf64-x86-64)
else ()
	set(TARGET_FORMAT elf-i386)
endif ()

# This custom commands creates a boot directory where the kernel will be located
# at, then runs a script that dumps necessary sections from the i386 image and
# converts them into the target format. After that the linker links the dumped
# sections with the kernel_cross and kernel_arch_x86 libraries into a single
# final kernel image.
# This complex solution is needed to correctly link 32bit i386 image with 64bit
# main kernel image in case of 64bit mode build.
add_custom_command(
	OUTPUT ${KERNEL_PATH}
	COMMAND ${CMAKE_COMMAND} -E make_directory ${KERNEL_DIR}
	COMMAND sh -c "${CMAKE_CURRENT_SOURCE_DIR}/scripts/dump_sections_i386.sh ${KERNEL_ARCH_i386_IMAGE} ${TARGET_FORMAT} | xargs ${CMAKE_LINKER} --whole-archive $<TARGET_FILE:kernel_cross> $<TARGET_FILE:kernel_arch_x86> -o ${KERNEL_PATH} -T ${LINKER_SCRIPT} --defsym __stack_size=${CONFIG_STACK_SIZE} --defsym __kernel_start=${CONFIG_ARCH_x86_KERNEL_START}"
	DEPENDS ${KERNEL_ARCH_i386_IMAGE} kernel_arch_i386 kernel_arch_x86 kernel_cross ${LINKER_SCRIPT}
	COMMENT "Linking uncompressed kernel image"
	VERBATIM)

add_custom_target(${TARGET_NAME} ALL DEPENDS ${KERNEL_PATH})
