set(x86_INCLUDE_DIRS 	${CMAKE_CURRENT_SOURCE_DIR}/include
			${CMAKE_CURRENT_BINARY_DIR}/generated/include)

# Declare x86-specific configs.
if (${CONFIG_ARCH} STREQUAL x86_64)
	decl_config(CONFIG_x86_PAGE_MAP_LEVEL PAGE_MAP_LEVEL_4)
	decl_config(CONFIG_PAGE_SIZE 0x1000)
else ()
	decl_config(CONFIG_x86_PAGE_MAP_LEVEL PAGE_MAP_LEVEL_2)
	decl_config(CONFIG_PAGE_SIZE 0x1000)
endif ()
set(CONFIG_x86_PAGE_MAP_LEVEL "x86_${CONFIG_x86_PAGE_MAP_LEVEL}")

decl_config(CONFIG_x86_PHYS_ADDR_64BIT ON)

if (CONFIG_x86_PHYS_ADDR_64BIT)
	set(CONFIG_x86_PHYS_ADDR_64BIT 1)
else ()
	set(CONFIG_x86_PHYS_ADDR_64BIT 0)
endif ()

# Configure arch/config.h.in file.
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/x86/config.h.in
	${CMAKE_CURRENT_BINARY_DIR}/generated/include/x86/config.h)

add_subdirectory(i386)
add_subdirectory(boot)

# Build x86 architecture specific library.
set(TARGET_NAME kernel_x86)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcmodel=large")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcmodel=large")

add_library(${TARGET_NAME} STATIC page_map.cc)
set_target_properties(${TARGET_NAME} PROPERTIES PREFIX "" SUFFIX ".elf")
target_include_directories(${TARGET_NAME} PUBLIC
	${x86_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${KLIBC_INCLUDE_DIR})

set(TARGET_NAME kernel_arch)
add_library(${TARGET_NAME} ALIAS kernel_x86)

# Build the uncompressed kernel image.
set(TARGET_NAME kernel)
set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld)

# Choose the target file format of the image.
if (${CONFIG_ARCH} STREQUAL x86_64)
	set(TARGET_FORMAT elf64-x86-64)
else ()
	set(TARGET_FORMAT elf-i386)
endif ()

set(DUMP_SECTION_i386_SCRIPT
		${CMAKE_CURRENT_SOURCE_DIR}/scripts/dump_sections_i386.sh)
set(SECTION_MEMLAYOUT ${CMAKE_CURRENT_SOURCE_DIR}/scripts/section_memlayout.S)

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
# 	COMMAND ${CMAKE_OBJCOPY} --set-section-flags .ldsym=alloc,readonly ${KERNEL_PATH} ${KERNEL_PATH}
# 	DEPENDS ${KERNEL_ARCH_i386_IMAGE} kernel_arch_i386 kernel_arch_x86 kernel_cross ${LINKER_SCRIPT} ${DUMP_SECTION_i386_SCRIPT} ${SECTION_MEMLAYOUT}
# 	COMMENT "Linking uncompressed kernel image"
# 	VERBATIM)
#
# add_custom_target(${TARGET_NAME} ALL DEPENDS ${KERNEL_PATH})
