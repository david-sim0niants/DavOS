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
	set(TARGET_NAME boot_grub)
	set(TARGET_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
	set(TARGET_OUTPUT_FILE ${TARGET_OUTPUT_DIR}/kernel.iso)

	set(GRUB_CFG ${CMAKE_CURRENT_SOURCE_DIR}/grub.cfg)
	set(TARGET_BOOT_DIR ${CMAKE_CURRENT_BINARY_DIR}/boot)
	set(TARGET_GRUB_DIR ${TARGET_BOOT_DIR}/grub)

	add_custom_command(
		OUTPUT ${TARGET_OUTPUT_FILE}
		COMMAND ${CMAKE_COMMAND} -E remove ${TARGET_OUTPUT_FILE}
		COMMAND ${CMAKE_COMMAND} -E make_directory ${TARGET_GRUB_DIR}
		COMMAND ${CMAKE_COMMAND} -E copy ${GRUB_CFG} ${TARGET_GRUB_DIR}
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:kernel> ${TARGET_BOOT_DIR}
		COMMAND ${GRUB_MKRESCUE_PATH} --xorriso=${XORRISO_PATH} ${TARGET_OUTPUT_DIR} -o ${TARGET_OUTPUT_FILE}
		DEPENDS kernel ${GRUB_CFG}
		COMMENT "Creating kernel.iso bootable image with GRUB.")

	add_custom_target(${TARGET_NAME} DEPENDS ${TARGET_OUTPUT_FILE})
	set_target_properties(${TARGET_NAME} PROPERTIES EXCLUDE_FROM_ALL 1)
else()
	add_custom_target(${TARGET_NAME}
		COMMAND echo "Error: grub-mkrescue or xorriso were not found during configuration."
		COMMAND false)
endif()

