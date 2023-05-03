set(TARGET_NAME kernel_arch_x86_protected_mode)

add_library(${TARGET_NAME} STATIC entry.S init.c vga_text.c)
set_target_properties(${TARGET_NAME} PROPERTIES PREFIX "" SUFFIX ".elf")

if (${CONFIG_ARCH_BITNESS} EQUAL 64)
	target_compile_options(${TARGET_NAME} PRIVATE -m32)
endif ()

add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
	COMMAND ${CMAKE_OBJCOPY} -O elf64-x86-64
		$<TARGET_FILE:${TARGET_NAME}> $<TARGET_FILE:${TARGET_NAME}>)


set(TARGET_NAME kernel_arch_x86)

add_library(${TARGET_NAME} STATIC null.c)
set_target_properties(${TARGET_NAME} PROPERTIES PREFIX "" SUFFIX ".elf")


set(TARGET_NAME kernel)

set(TARGET_STATIC_LIBS
	kernel_arch_x86_protected_mode kernel_arch_x86 kernel_cross)
set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld)
set(TARGET_LINK_INPUTS "")

# foreach(TARGET_LIB ${TARGET_STATIC_LIBS})
# 	list(APPEND TARGET_LINK_INPUTS -L$<TARGET_FILE_DIR:${TARGET_LIB}>)
# endforeach(TARGET_LIB)

foreach(TARGET_LIB ${TARGET_STATIC_LIBS})
	list(APPEND TARGET_LINK_INPUTS $<TARGET_FILE:${TARGET_LIB}>)
endforeach(TARGET_LIB)

list(APPEND TARGET_LINK_INPUTS -T${LINKER_SCRIPT})


add_custom_command(
	OUTPUT ${KERNEL_PATH}
	COMMAND ${CMAKE_COMMAND} -E make_directory ${KERNEL_DIR}
	COMMAND ${CMAKE_LINKER} ${TARGET_LINK_INPUTS} -o ${KERNEL_PATH} --gc-sections
	DEPENDS ${TARGET_STATIC_LIBS} ${LINKER_SCRIPT}
	COMMENT "Linking uncompressed kernel image")

add_custom_target(${TARGET_NAME} ALL DEPENDS ${KERNEL_PATH})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
