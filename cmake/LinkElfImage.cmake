function(add_elf_image TARGET_NAME OBJECT_LIB)
	set(TARGET_OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.elf)

	if (ARGC GREATER 2)
		set(LINKER_SCRIPT ARG2)
		set(LINK_SCRIPT_OPTION -T ${LINKER_SCRIPT})
	else()
		set(LINK_SCRIPT_OPTION "")
    	endif()

    	add_custom_command(
		OUTPUT ${TARGET_OUTPUT_FILE}
		COMMAND
		${CMAKE_LINKER} ${LINK_SCRIPT_OPTION}
		$<TARGET_OBJECTS:${OBJECT_LIBS}> -o ${TARGET_OUTPUT_FILE}
		DEPENDS ${OBJECT_LIBS} ${LINKER_SCRIPT})

    	add_library(${TARGET_NAME} STATIC IMPORTED GLOBAL)
    	set_target_properties(${TARGET_NAME}
		PROPERTIES IMPORTED_LOCATION ${TARGET_OUTPUT_FILE})
    	add_dependencies(${TARGET_NAME} ${TARGET_OUTPUT_FILE})
endfunction()
