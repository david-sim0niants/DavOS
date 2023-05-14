# Build objects for i386 specific image
set(TARGET_NAME kernel_arch_i386_objs)
set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/linker_i386.ld)

add_library(${TARGET_NAME} OBJECT entry.S ldconfig.S init.c ../vga_text.c)
if (${CONFIG_ARCH} STREQUAL x86_64)
	target_compile_options(${TARGET_NAME} PRIVATE -m32)
endif ()


# Build the i386 specific image, responsible for containing multiboot 2 header
# to be recognizable by multiboot 2 compatible bootloaders. The image will also
# initialize x86 architecture, in case of x86_64 build, will try to switch into
# long mode and finally jump onto the kernel start.
set(TARGET_NAME kernel_arch_i386)
set(TARGET_OUTPUT_FILE_NAME ${TARGET_NAME}.elf)
set(TARGET_OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_OUTPUT_FILE_NAME})
set(KERNEL_ARCH_i386_IMAGE ${TARGET_OUTPUT_FILE})

# Custom command that links object files together and forms the i386 image.
add_custom_command(
	OUTPUT ${TARGET_OUTPUT_FILE}
	COMMAND ${CMAKE_LINKER} $<TARGET_OBJECTS:kernel_arch_i386_objs> -T ${LINKER_SCRIPT} -melf_i386 --gc-sections -o ${TARGET_OUTPUT_FILE} --defsym __stack_size=${CONFIG_STACK_SIZE}
	DEPENDS kernel_arch_i386_objs $<TARGET_OBJECTS:kernel_arch_i386_objs> ${LINKER_SCRIPT}
	COMMENT "Linking ${TARGET_OUTPUT_FILE_NAME}"
	VERBATIM COMMAND_EXPAND_LISTS)

add_custom_target(${TARGET_NAME} DEPENDS ${TARGET_OUTPUT_FILE} $<TARGET_OBJECTS:kernel_arch_i386_objs>)

set(KERNEL_ARCH_i386_IMAGE ${KERNEL_ARCH_i386_IMAGE} PARENT_SCOPE)
