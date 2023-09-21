# Here a multiboot2 compliant kernel is built for x86 architecture

# Build i386-specific portion of the kernel entry code. 
#
set(TARGET_NAME kernel_entry_i386)

# Binaries to include in custom linking
set(LINK_INPUTS kernel_x86_mb2_entry kernel_x86_i386_entry)

# Template of linker script for linking i386-specific code
set(LINKER_SCRIPT_IN 	${CMAKE_CURRENT_SOURCE_DIR}/i386_entry.ld.in)
# Linker script for linking i386-specific code
set(LINKER_SCRIPT	${CMAKE_CURRENT_BINARY_DIR}/i386_entry.ld)

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
	COMMENT "Linking i386-specific kernel entry image.")

# Generate a linker script from its template prior to linking
set(INC_DIRS ${ROOT_INCLUDE_DIRS})
list(TRANSFORM INC_DIRS PREPEND "-I")
add_custom_command(TARGET ${TARGET_NAME} PRE_LINK
	COMMAND ${CPREPROCESSOR} ${LINKER_SCRIPT_IN} ${INC_DIRS} -P -o ${LINKER_SCRIPT})


set(TARGET_NAME kernel_entry_i386_dumped_sections)

set(DUMP_SECTIONS_SCRIPT ${CMAKE_SOURCE_DIR}/scripts/dump_sections/main.sh)

set(DUMPED_SECTIONS .text .data .rodata)
set(DUMPED_SECTION_FILES "")
set(DUMPED_SECTION_FN_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/kernel_entry_i386)

foreach(SECTION ${DUMPED_SECTIONS})
	set(SECTION_FILE ${DUMPED_SECTION_FN_PREFIX}${SECTION}.o)
	list(APPEND DUMPED_SECTION_FILES ${SECTION_FILE})
endforeach()

add_custom_command(OUTPUT ${DUMPED_SECTION_FILES}
	COMMAND ${BASH} ${DUMP_SECTIONS_SCRIPT} $<TARGET_FILE:kernel_entry_i386> i386 ${DUMPED_SECTION_FN_PREFIX} ${CONFIG_ARCH_BITNESS} ${CMAKE_C_COMPILER} ${CMAKE_OBJCOPY}
	DEPENDS kernel_entry_i386
	VERBATIM)

add_custom_target(${TARGET_NAME} ALL DEPENDS ${DUMPED_SECTION_FILES})


# Build the complete kernel entry code.
set(TARGET_NAME kernel_entry)

get_null_file(cpp)
add_executable(${TARGET_NAME} ${NULL_FILE})

set(LINKER_SCRIPT_IN 	${CMAKE_CURRENT_SOURCE_DIR}/entry.ld.in)
set(LINKER_SCRIPT	${CMAKE_CURRENT_BINARY_DIR}/entry.ld)

set(LINK_OTHER_OPTIONS	"--gc-sections,--build-id=none")
set(LINK_OPTIONS "LINKER:,-T${LINKER_SCRIPT},${LINK_OTHER_OPTIONS}")

set(INC_DIRS ${ROOT_INCLUDE_DIRS})
list(TRANSFORM INC_DIRS PREPEND "-I")
add_custom_command(TARGET kernel_entry PRE_LINK
	COMMAND ${CPREPROCESSOR} ${LINKER_SCRIPT_IN} ${INC_DIRS} -P -o ${LINKER_SCRIPT}
	DEPENDS kernel_entry_i386)

target_link_libraries(${TARGET_NAME} PRIVATE ${DUMPED_SECTION_FILES})
target_link_options(${TARGET_NAME} PRIVATE ${LINK_OPTIONS})
set_target_properties(${TARGET_NAME} PROPERTIES
	LINK_DEPENDS ${LINKER_SCRIPT_IN}
	COMMENT "Linking kernel entry.")
add_dependencies(${TARGET_NAME} kernel_entry_i386_dumped_sections)


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
