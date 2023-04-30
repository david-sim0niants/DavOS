find_program(GRUB_MKRESCUE_PATH NAMES grub-mkrescue)

if (GRUB_MKRESCUE_PATH)
	message(STATUS "grub-mkrescue is installed.")

	find_program(XORRISO_PATH NAMES xorriso)
	if (XORRISO_PATH)
		message(STATUS "xorriso is installed.")
	else()
		message(STATUS "xorriso is NOT installed.")
	endif()
else()
	message(STATUS "grub-mkrescue is NOT installed.")
endif()


if (GRUB_MKRESCUE_PATH AND XORRISO_PATH)

	set(TARGET_NAME kernel_iso)
	set(TARGET_OUTPUT_FILE ${KERNEL_DIR}/../kernel.iso)

	add_custom_command(
		OUTPUT ${TARGET_OUTPUT_FILE}
		COMMAND ${CMAKE_COMMAND} -E make_directory ${KERNEL_DIR}/grub
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/grub/ ${KERNEL_DIR}/grub
		COMMAND ${GRUB_MKRESCUE_PATH} --xorriso=${XORRISO_PATH} ${KERNEL_DIR}/../ -o ${TARGET_OUTPUT_FILE}
		DEPENDS ${KERNEL_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/grub/grub.cfg
		COMMENT "Creating ISO image")

	add_custom_target(${TARGET_NAME} DEPENDS ${TARGET_OUTPUT_FILE})
	set_target_properties(${TARGET_NAME} PROPERTIES EXCLUDE_FROM_ALL 1)
else()
	add_custom_target(${TARGET_NAME}
		COMMAND echo "Error: grub-mkrescue or xorriso were not found during configuration."
		COMMAND false)
endif()

