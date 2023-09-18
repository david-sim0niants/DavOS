# Here a multiboot2 compliant kernel is built for x86 architecture

# Build i386-specific portion of the kernel entry code. 
#
set(TARGET_NAME kernel_entry_i386)

# Binaries to include in custom linking
set(LINK_INPUTS kernel_x86_mb2_entry kernel_x86_i386_entry)

# Template of linker script for linking i386-specific code
set(LINKER_SCRIPT_IN 	${CMAKE_CURRENT_SOURCE_DIR}/i386.linker.ld.in)
# Linker script for linking i386-specific code
set(LINKER_SCRIPT	${CMAKE_CURRENT_BINARY_DIR}/i386.linker.ld)

set(LINK_OTHER_OPTIONS	"-melf_i386,--gc-sections,--build-id=none")
set(LINK_OPTIONS "LINKER:,-T${LINKER_SCRIPT},${LINK_OTHER_OPTIONS}")

# As we don't compile anything here and just do the linking stuff, and since cmake
# requires at least one source file to be specified, we just provide a null file.
get_null_file(cpp)
add_executable(${TARGET_NAME} ${NULL_FILE})
# Link the binaries.
target_link_libraries(${TARGET_NAME} PRIVATE ${LINK_INPUTS})

if (${CONFIG_ARCH} STREQUAL x86_64)
	# This is for the null file.
	target_compile_options(${TARGET_NAME} PRIVATE -m32 -mno-sse)
endif ()

target_link_options(${TARGET_NAME} PRIVATE ${LINK_OPTIONS})
set_target_properties(${TARGET_NAME} PROPERTIES
	LINK_DEPENDS ${LINKER_SCRIPT_IN}
	COMMENT "Linking i386-specific kernel image.")

# Generate a linker script from its template prior to linking
set(INC_DIRS ${ROOT_INCLUDE_DIRS})
list(TRANSFORM INC_DIRS PREPEND "-I")
add_custom_command(TARGET ${TARGET_NAME} PRE_LINK
	COMMAND ${CPREPROCESSOR} ${LINKER_SCRIPT_IN} ${INC_DIRS} -P -o ${LINKER_SCRIPT})


# Build the complete kernel entry code.
set(TARGET_NAME kernel_entry)

# Link i386-specific code with the general code and
# build the uncompressed kernel imge.
#
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
set(SECTION_MEMLAYOUT
		${CMAKE_CURRENT_SOURCE_DIR}/scripts/section_memlayout.S)
