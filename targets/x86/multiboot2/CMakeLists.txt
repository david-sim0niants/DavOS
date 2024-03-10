# Here a multiboot2 compliant kernel is built for x86 architecture


# Generate a linker script from its template prior to linking.
function(gen_linker_script_comm TARGET_NAME LINKER_SCRIPT_IN LINKER_SCRIPT)
	set(INC_DIRS ${ARGN})
	list(TRANSFORM INC_DIRS PREPEND "-I")
	add_custom_command(TARGET ${TARGET_NAME} PRE_LINK
		COMMAND ${CPREPROCESSOR} ${LINKER_SCRIPT_IN} ${INC_DIRS} -P -o ${LINKER_SCRIPT})
endfunction(gen_linker_script_comm)


# .ldsym section remains writable after linking, make it readonly using objcopy.
function(make_ldsym_readonly_comm TARGET_NAME)
	add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
		COMMAND ${CMAKE_OBJCOPY} --set-section-flags .ldsym=alloc,readonly $<TARGET_FILE:${TARGET_NAME}> $<TARGET_FILE:${TARGET_NAME}>
		VERBATIM)
endfunction(make_ldsym_readonly_comm)


set(DUMP_SECTIONS_SCRIPT     ${CMAKE_SOURCE_DIR}/scripts/dump_sections/main.sh)
set(DUMPED_SECTION_FN_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/kernel_entry_i386)

# Get paths to the dumped section files.
function(get_dumped_section_files RESULT_VAR)
	set(SECTIONS ${ARGN})
	set(DUMPED_SECTION_FILES "")

	foreach(SECTION ${SECTIONS})
		set(SECTION_FILE ${DUMPED_SECTION_FN_PREFIX}${SECTION}.o)
		list(APPEND DUMPED_SECTION_FILES ${SECTION_FILE})
	endforeach()

	set(${RESULT_VAR} ${DUMPED_SECTION_FILES} PARENT_SCOPE)
endfunction(get_dumped_section_files)


# Get paths to template and preprocessed linker scripts.
function(get_linker_script_path NAME TEMPLATE_FP_VAR PREPROC_FP_VAR)
	# Template linker script path
	set(${TEMPLATE_FP_VAR} 	${CMAKE_CURRENT_SOURCE_DIR}/${NAME}.ld.in PARENT_SCOPE)
	# Preprocessed linker script path
	set(${PREPROC_FP_VAR} 	${CMAKE_CURRENT_BINARY_DIR}/${NAME}.ld PARENT_SCOPE)
endfunction(get_linker_script_path)


# Build i386-specific portion of the kernel entry code. 
#
set(TARGET_NAME kernel_entry_i386)

# Binaries to include in custom linking
set(LINK_INPUTS kernel_x86_mb2_entry kernel_x86_i386_entry)
# Get paths to template and preprocessed linker scripts for i386-specific code.
get_linker_script_path(kernel_i386 LINKER_SCRIPT_IN LINKER_SCRIPT)

set(LINK_OTHER_OPTIONS	"-melf_i386,--gc-sections,--build-id=none")
set(LINK_OPTIONS "LINKER:,-T${LINKER_SCRIPT},${LINK_OTHER_OPTIONS}")

# As we don't compile anything here and just do the linking stuff, and since cmake
# requires at least one source file to be specified, we just provide a null file.
get_null_file(cpp)
add_executable(${TARGET_NAME} ${NULL_FILE})
# Link the binaries.
target_link_libraries(${TARGET_NAME} PRIVATE ${LINK_INPUTS})
target_compile_definitions(${TARGET_NAME} PRIVATE __KERNEL_ENTRY__)

if (${CONFIG_ARCH} STREQUAL x86_64)
	# This is for the null file.
	target_compile_options(${TARGET_NAME} PRIVATE -m32 -mno-sse)
endif ()

target_link_options(${TARGET_NAME} PRIVATE ${LINK_OPTIONS})
set_target_properties(${TARGET_NAME} PROPERTIES
	LINK_DEPENDS ${LINKER_SCRIPT_IN})

gen_linker_script_comm(${TARGET_NAME} ${LINKER_SCRIPT_IN} ${LINKER_SCRIPT} ${ROOT_INCLUDE_DIRS})
make_ldsym_readonly_comm(${TARGET_NAME})


# Build the complete kernel entry code.
#
set(TARGET_NAME kernel_entry)

set(DUMPED_SECTION_LABELS multiboot2_header text data rodata)
set(DUMPED_SECTIONS multiboot2_header:a text:ax data:wa rodata:a)

set(DUMPED_SECTION_NAMES ${DUMPED_SECTION_LABELS})
list(TRANSFORM DUMPED_SECTION_NAMES PREPEND .)

get_dumped_section_files(DUMPED_SECTION_FILES ${DUMPED_SECTION_NAMES})

add_custom_command(OUTPUT ${DUMPED_SECTION_FILES}
	COMMAND ${BASH} ${DUMP_SECTIONS_SCRIPT} $<TARGET_FILE:kernel_entry_i386> i386 ${DUMPED_SECTION_FN_PREFIX} ${CONFIG_ARCH_BITNESS} ${CMAKE_C_COMPILER} ${CMAKE_OBJCOPY} ${DUMPED_SECTIONS}
	COMMENT "Dumping sections of kernel_entry_i386"
	DEPENDS ${DUMP_SECTIONS_SCRIPT} kernel_entry_i386
	VERBATIM)

get_null_file(cpp)
add_library(${TARGET_NAME} INTERFACE)
target_sources(${TARGET_NAME} INTERFACE ${NULL_FILE} ${DUMPED_SECTION_FILES})
add_dependencies(${TARGET_NAME} kernel_entry_i386)
if (${CONFIG_ARCH} STREQUAL x86_64)
	target_link_libraries(${TARGET_NAME} INTERFACE kernel_entry_x86_64)
endif ()


# Build the final kernel image
#
set(TARGET_NAME kernel)

get_null_file(cpp)
add_executable(${TARGET_NAME} ${NULL_FILE})

# Get paths to template and preprocessed linker scripts for the final kernel.
get_linker_script_path(kernel LINKER_SCRIPT_IN LINKER_SCRIPT)

set(LINK_OTHER_OPTIONS	"--gc-sections,--build-id=none,--whole-archive")
set(LINK_OPTIONS "LINKER:,-T${LINKER_SCRIPT},${LINK_OTHER_OPTIONS}")

target_link_libraries(${TARGET_NAME} PRIVATE kernel_entry kernel_main)
target_link_options(${TARGET_NAME} PRIVATE ${LINK_OPTIONS})
set_target_properties(${TARGET_NAME} PROPERTIES
	LINK_DEPENDS ${LINKER_SCRIPT_IN})

gen_linker_script_comm(${TARGET_NAME} ${LINKER_SCRIPT_IN} ${LINKER_SCRIPT} ${ROOT_INCLUDE_DIRS})
make_ldsym_readonly_comm(${TARGET_NAME})
