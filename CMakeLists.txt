cmake_minimum_required(VERSION 3.12)
project(DavOS ASM C)

set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -x assembler-with-cpp")

if (${CMAKE_SIZEOF_VOID_P} STREQUAL 4)
	set(ARCH_BITNESS 32)
elseif (${CMAKE_SIZEOF_VOID_P} STREQUAL 8)
	set(ARCH_BITNESS 64)
else ()
	message(FATAL_ERROR "Specified architecture is neither 32, nor 64 bit.")
endif ()

message(STATUS
	"Target architecture: ${CMAKE_SYSTEM_PROCESSOR} ${ARCH_BITNESS}bit")


set(CMAKE_C_FLAGS "-fno-ident -ffreestanding -fno-exceptions -nostdlib -nostdinc -fno-stack-protector")


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(LinkElfImage)

add_subdirectory(arch)
add_subdirectory(kernel)
add_subdirectory(boot)

