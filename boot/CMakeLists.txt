set(TARGET_NAME kernel_iso)

find_program(GRUB_MKRESCUE_COMM NAMES grub-mkrescue)

if (GRUB_MKRESCUE_COMM)
	message(STATUS "grub-mkrescue is installed.")
	add_custom_target(${TARGET_NAME}
		COMMAND ${CMAKE_COMMAND} -E make_directory ${KERNEL_DIR}/grub
		COMMAND ${CMAKE_COMMAND} -E copy_directory
			${CMAKE_CURRENT_SOURCE_DIR}/grub/ ${KERNEL_DIR}/grub
		COMMAND grub-mkrescue ${KERNEL_DIR} -o ${KERNEL_DIR}/kernel.iso)
else()
	message(WARNING "grub-mkrescue is NOT installed.")
	add_custom_target(${TARGET_NAME}
		COMMAND echo "Error: grub-mkrescue is not installed"
		COMMAND false)
endif()

