# Setup x86 include directories.
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
add_subdirectory(utils)

# Build x86 architecture specific library.
set(TARGET_NAME kernel_x86)

add_library(${TARGET_NAME} INTERFACE)
target_sources(${TARGET_NAME} INTERFACE page_map.cc)
set(INC_DIRS ${x86_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS})
target_include_directories(${TARGET_NAME} INTERFACE ${INC_DIRS})
target_link_libraries(${TARGET_NAME} INTERFACE klibc)

if (${CONFIG_ARCH} STREQUAL x86_64)
	target_compile_options(${TARGET_NAME} INTERFACE -mcmodel=kernel)
endif ()
