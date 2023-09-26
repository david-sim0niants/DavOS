if (CONFIG_ARCH STREQUAL x86_64)
	set(CMAKE_C_CXX_FLAGS "${CMAKE_C_FLAGS} -mcmodel=kernel")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_C_CXX_FLAGS}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_CXX_FLAGS}")
endif ()

# Build arch-independent portion of the kernel.
set(TARGET_NAME kernel_cross)
add_library(${TARGET_NAME} STATIC main.cc)

# Build the main, independent from its entry portion of the kernel.
set(TARGET_NAME kernel_main)
get_null_file(cpp)
add_library(${TARGET_NAME} STATIC ${NULL_FILE} $<TARGET_OBJECTS:kernel_cross> $<TARGET_OBJECTS:kernel_arch>)
# target_link_libraries(${TARGET_NAME} PRIVATE kernel_cross kernel_arch)
