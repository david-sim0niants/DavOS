cmake_minimum_required(VERSION 3.12)
project(DavOS ASM C)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(DeclareConfig)

# Determine the target architecture and its bitness
if (CMAKE_SYSTEM_PROCESSOR MATCHES "(x86_64|X86_64|x64|X64|amd64|AMD64|ia32e|IA32e)")

	decl_config(CONFIG_ARCH x86_64 "Target architecture.")
	decl_config(CONFIG_ARCH_BITNESS 64 "Target architecture bitness.")

elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "(i386|x86|X86|ia32|IA32)")
	decl_config(CONFIG_ARCH i386 "Target architecture.")
	decl_config(CONFIG_ARCH_BITNESS 32 "Target architecture bitness.")
endif()

message(STATUS "Target architecture: ${CONFIG_ARCH} ${CONFIG_ARCH_BITNESS}bit")


# Set compiler flags
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -x assembler-with-cpp --no-pic --freestanding --no-standard-libraries --no-standard-includes --no-stack-protector")
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -fno-pic -fno-ident -ffreestanding -fno-exceptions -nostdlib -nostdinc -fno-stack-protector")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-pic -ffreestanding -fno-exceptions -fno-rtti -nostdlib -nostdinc")

set(CONFIG_STACK_SIZE 0x2000 CACHE STRING "Stack size.")

if (CONFIG_ARCH_BITNESS EQUAL 64)
	decl_config(CONFIG_VM_SPLIT 0x800000000000 "Stack size.")
else ()
	decl_config(CONFIG_VM_SPLIT 0xC0000000 "Stack size.")
endif ()


# Include include/ and generated include/ directories.
include_directories(include)
include_directories(${CMAKE_BINARY_DIR}/generated/include)

add_subdirectory(arch)
add_subdirectory(kernel)
add_subdirectory(boot)

# Configure config.h.in file.
set(CONFIG_ARCH_MACRO ARCH_${CONFIG_ARCH})
configure_file(${CMAKE_SOURCE_DIR}/include/config.h.in
	${CMAKE_BINARY_DIR}/generated/include/config.h)
