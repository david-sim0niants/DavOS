if (CONFIG_ARCH STREQUAL x86_64)
	set(CMAKE_C_CXX_FLAGS "${CMAKE_C_FLAGS} -mcmodel=kernel")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_C_CXX_FLAGS}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_CXX_FLAGS}")
endif ()

# Build 'cross-architecture' part of the kernel as a static library.
set(TARGET_NAME kernel_cross)

add_library(${TARGET_NAME} STATIC main.cc)
set_target_properties(${TARGET_NAME} PROPERTIES PREFIX "" SUFFIX ".elf")

set(TARGET_NAME kernel_main)
get_null_file(cpp)
add_library(${TARGET_NAME} STATIC ${NULL_FILE})
target_link_libraries(${TARGET_NAME} PUBLIC kernel_cross kernel_arch)
