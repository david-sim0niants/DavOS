if (CONFIG_ARCH STREQUAL x86_64)
	set(CMAKE_C_CXX_FLAGS "${CMAKE_C_FLAGS} -mcmodel=kernel")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_C_CXX_FLAGS}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_CXX_FLAGS}")
endif ()

# Build the main, independent from its entry portion of the kernel.
set(TARGET_NAME kernel_main)
add_library(${TARGET_NAME} INTERFACE)
target_sources(${TARGET_NAME} INTERFACE main.cc)
target_link_libraries(${TARGET_NAME} INTERFACE kernel_arch)
