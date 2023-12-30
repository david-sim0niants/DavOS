set(ARCH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (${CONFIG_ARCH} STREQUAL i386 OR ${CONFIG_ARCH} STREQUAL x86_64)
	set(ARCH_DIR x86)
endif()

add_subdirectory(${ARCH_DIR})

add_subdirectory(bridge)

# Build kernel_arch library that wraps kernel_x86 library in a cross-platform API.
#
set(TARGET_NAME kernel_arch)
add_library(${TARGET_NAME} INTERFACE)
target_link_libraries(${TARGET_NAME} INTERFACE kernel_x86 kernel_arch_bridge)
