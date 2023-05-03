cmake_minimum_required(VERSION 3.12)
project(DavOS ASM C)


# Determine the target architecture and its bitness
if (CMAKE_SYSTEM_PROCESSOR MATCHES "(i386|x86|X86|ia32|IA32)")
	set(CONFIG_ARCH i386)
	set(CONFIG_ARCH_BITNESS 32)
endif()

if (CMAKE_SYSTEM_PROCESSOR MATCHES "(x86_64|X86_64|x64|X64|amd64|AMD64)")
	set(CONFIG_ARCH x86_64)
	set(CONFIG_ARCH_BITNESS 64)
endif()

message(STATUS "Target architecture: ${CONFIG_ARCH} ${CONFIG_ARCH_BITNESS}bit")


# Set compiler flags
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -x assembler-with-cpp --no-pic --freestanding --no-standard-libraries --no-standard-includes --no-stack-protector")
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -fno-pic -fno-ident -ffreestanding -fno-exceptions -nostdlib -nostdinc -fno-stack-protector")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-pic -ffreestanding -fno-exceptions -fno-rtti -nostdlib -nostdinc")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include_directories(include)
include_directories(${CMAKE_BINARY_DIR}/generated/include)

add_subdirectory(arch)
add_subdirectory(kernel)
add_subdirectory(boot)

# Configure the config header file
configure_file(${CMAKE_SOURCE_DIR}/include/config.h.in
	${CMAKE_BINARY_DIR}/generated/include/config.h)
