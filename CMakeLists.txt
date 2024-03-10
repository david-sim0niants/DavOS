cmake_minimum_required(VERSION 3.13)
project(DavOS ASM C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

# Find the C preprocessor
find_program(CPREPROCESSOR NAMES cpp)
if (NOT CPREPROCESSOR)
	message(FATAL_ERROR "C pre-processor not found.")
endif ()

# Find bash shell
find_program(BASH NAMES bash)
if (NOT BASH)
	message(FATAL_ERROR "bash not found.")
endif ()

# Include some utilities
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(DeclareConfig)
include(SwitchMode)
include(NullFile)

# Include config.cmake file containing generated configuration.
if (EXISTS ${CMAKE_BINARY_DIR}/config.cmake)
	include(${CMAKE_BINARY_DIR}/config.cmake)
endif ()

# Determine the target architecture and its bitness
if (CMAKE_SYSTEM_PROCESSOR MATCHES "(x86_64|X86_64|x64|X64|amd64|AMD64|ia32e|IA32e)")
	decl_config(CONFIG_ARCH x86_64)
	decl_config(CONFIG_ARCH_BITNESS 64)
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "(i386|x86|X86|ia32|IA32)")
	decl_config(CONFIG_ARCH i386)
	decl_config(CONFIG_ARCH_BITNESS 32)
endif()

message(STATUS "Target architecture: ${CONFIG_ARCH} ${CONFIG_ARCH_BITNESS}bit")

decl_config(CONFIG_STACK_SIZE 0x2000)
decl_config(CONFIG_STACK_ALIGNMENT 0x1000)

if (CONFIG_ARCH_BITNESS EQUAL 64)
	decl_config(CONFIG_VM_SPLIT 0x800000000000)
else ()
	decl_config(CONFIG_VM_SPLIT 0xC0000000)
endif ()

# Setup root include directories which are the top level include and
# generated include directories.
set(ROOT_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/include ${CMAKE_BINARY_DIR}/generated/include)
# Setup the kernel include directory.
set(KERNEL_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/kernel/include)
# Setup the kernel libc library include directory.
set(KLIBC_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/klibc/include)
# Setup the arch API library include directory.
set(ARCH_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/arch/include)
# Setup the targets include directory
set(TARGETS_INLUDE_DIR ${CMAKE_SOURCE_DIR}/targets)

# Switch to mode with CMake configuration setup for building the kernel.
switch_mode(KERNEL)

add_subdirectory(arch)
add_subdirectory(boot)
add_subdirectory(kernel)
add_subdirectory(klibc)
add_subdirectory(targets)

decl_config(CONFIG_HAVE_TESTS OFF)
if (CONFIG_HAVE_TESTS)
	switch_mode(USER)
	add_subdirectory(test)
endif()

# Configure config.h.in file.
set(CONFIG_ARCH_MACRO ARCH_${CONFIG_ARCH})
configure_file(${CMAKE_SOURCE_DIR}/include/config.h.in
	${CMAKE_BINARY_DIR}/generated/include/config.h)
