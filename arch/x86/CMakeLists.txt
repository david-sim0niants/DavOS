set(TARGET_NAME kernel_arch_x86_objs)
add_library(${TARGET_NAME} OBJECT entry.S init.c)


set(TARGET_NAME kernel_static)
set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld)

add_library(${TARGET_NAME} STATIC
	$<TARGET_OBJECTS:kernel_arch_x86_objs> $<TARGET_OBJECTS:kernel_objs>)
set_target_properties(${TARGET_NAME} PROPERTIES PREFIX "" SUFFIX ".elf")

set(TARGET_NAME kernel)

add_custom_command(
	OUTPUT ${KERNEL_PATH}
	COMMAND ${CMAKE_COMMAND} -E make_directory ${KERNEL_DIR}
	COMMAND ${CMAKE_LINKER} -L $<TARGET_FILE_DIR:kernel_static>
		$<TARGET_FILE_NAME:kernel_static> -T ${LINKER_SCRIPT}
		-o ${KERNEL_PATH} --gc-sections
	DEPENDS kernel_static ${LINKER_SCRIPT}
	COMMENT "Linking uncompressed kernel image")

add_custom_target(${TARGET_NAME} ALL DEPENDS ${KERNEL_PATH})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
