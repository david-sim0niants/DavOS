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

add_library(${TARGET_NAME} STATIC page_map.cc)
set(INC_DIRS ${x86_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${KLIBC_INCLUDE_DIR})
target_include_directories(${TARGET_NAME} PUBLIC ${INC_DIRS})

if (${CONFIG_ARCH} STREQUAL x86_64)
	target_compile_options(${TARGET_NAME} PRIVATE -mcmodel=kernel)
endif ()

set(TARGET_NAME kernel_arch)
add_library(${TARGET_NAME} ALIAS kernel_x86)
