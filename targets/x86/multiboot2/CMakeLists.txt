# Here a multiboot2 compliant kernel is built for x86 architecture

# Build i386-specific portion of the kernel entry code. 
#
set(TARGET_NAME kernel_entry_i386)
set(TARGET_OUTPUT_FILE_NAME ${TARGET_NAME}.elf)
set(TARGET_OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_OUTPUT_FILE_NAME})

# Libraries to include in custom linking
set(TARGET_LINK_INPUTS kernel_x86_mb2_entry kernel_x86_i386_entry)

# Linker script for linking i386-specific code
set(LINKER_SCRIPT	${CMAKE_CURRENT_SOURCE_DIR}/i386.linker.ld)

set(LINK_INPUTS 	"")
foreach(LIB ${TARGET_LINK_INPUTS})
	list(APPEND LINK_INPUTS $<TARGET_FILE:${LIB}>)
endforeach(LIB ${TARGET_LINK_LIBRARIES})

set(LINK_SYM_DEFS	--defsym __stack_size=${CONFIG_STACK_SIZE})

set(LINK_OTHER_FLAGS	-melf_i386 --gc-sections)

# Custom command that links object files together and forms the i386 image.
add_custom_command(
	OUTPUT ${TARGET_OUTPUT_FILE}
	COMMAND "${CMAKE_LINKER} ${LINK_INPUTS} -o${TARGET_OUTPUT_FILE} \
		${LINK_SYM_DEFS} -T${LINKER_SCRIPT} ${LINK_OTHER_FLAGS}"
	COMMAND ${CMAKE_OBJCOPY} --set-section-flags .ldconfig=alloc,readonly ${TARGET_OUTPUT_FILE} ${TARGET_OUTPUT_FILE}
	DEPENDS ${TARGET_LINK_LIBRARIES} ${LINKER_SCRIPT}
	COMMENT "Linking ${TARGET_OUTPUT_FILE_NAME}"
	VERBATIM COMMAND_EXPAND_LISTS)

add_custom_target(${TARGET_NAME} DEPENDS ${TARGET_OUTPUT_FILE} ${TARGET_LINK_INPUTS})

set(TARGET_NAME kernel_entry)
add_custom_target(${TARGET_NAME} DEPENDS kernel_entry_i386)



# Link i386-specific code with the general code and
# build the uncompressed kernel imge.
#
set(TARGET_NAME kernel)
set(TARGET_OUTPUT_FILE ${CMAKE_CURRENT_SOURCE_DIR})
set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld)

# Choose the target file format of the image.
if (${CONFIG_ARCH} STREQUAL x86_64)
	set(TARGET_FORMAT elf64-x86-64)
else ()
	set(TARGET_FORMAT elf-i386)
endif ()

set(DUMP_SECTION_i386_SCRIPT
		${CMAKE_CURRENT_SOURCE_DIR}/scripts/dump_sections_i386.sh)
set(SECTION_MEMLAYOUT
		${CMAKE_CURRENT_SOURCE_DIR}/scripts/section_memlayout.S)

# add_custom_command(
# 	OUTPUT ${KERNEL_})

# # This custom command creates a boot directory where the kernel will be located
# # at, then runs a script that dumps necessary sections from the i386 image and
# # converts them into the target format. After that the linker links the dumped
# # sections with the kernel_cross and kernel_arch_x86 libraries into a single
# # final kernel image.
# # This complex solution is needed to correctly link 32bit i386 image with 64bit
# # main kernel image in case of 64bit mode build.
# add_custom_command(
# 	OUTPUT ${KERNEL_PATH}
# 	COMMAND ${CMAKE_COMMAND} -E make_directory ${KERNEL_DIR}
# 	COMMAND sh -c "${DUMP_SECTION_i386_SCRIPT} ${KERNEL_ARCH_i386_IMAGE} ${TARGET_FORMAT} ${CMAKE_C_COMPILER} ${CMAKE_OBJCOPY} | xargs ${CMAKE_LINKER} --whole-archive $<TARGET_FILE:kernel_cross> $<TARGET_FILE:kernel_arch_x86> -o ${KERNEL_PATH} -T ${LINKER_SCRIPT} --defsym __STACK_SIZE=${CONFIG_STACK_SIZE} --defsym __VM_SPLIT=${CONFIG_VM_SPLIT}"
# 	COMMAND ${CMAKE_OBJCOPY} --set-section-flags .ldconfig=alloc,readonly ${KERNEL_PATH} ${KERNEL_PATH}
# 	DEPENDS ${KERNEL_ARCH_i386_IMAGE} kernel_arch_i386 kernel_arch_x86 kernel_cross ${LINKER_SCRIPT} ${DUMP_SECTION_i386_SCRIPT} ${SECTION_MEMLAYOUT}
# 	COMMENT "Linking uncompressed kernel image"
# 	VERBATIM)
#
# add_custom_target(${TARGET_NAME} ALL DEPENDS ${KERNEL_PATH})
