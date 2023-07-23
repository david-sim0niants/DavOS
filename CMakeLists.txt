cmake_minimum_required(VERSION 3.12)
project(DavOS ASM C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

set(DEFAULT_CMAKE_ASM_FLAGS ${CMAKE_ASM_FLAGS})
set(DEFAULT_CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
set(DEFAULT_CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})


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

decl_config(CONFIG_STACK_SIZE 0x2000 "Stack size.")

if (CONFIG_ARCH_BITNESS EQUAL 64)
	decl_config(CONFIG_VM_SPLIT 0x800000000000 "Stack size.")
else ()
	decl_config(CONFIG_VM_SPLIT 0xC0000000 "Stack size.")
endif ()


set(IMAGE_DIR ${CMAKE_BINARY_DIR}/image)
set(KERNEL_DIR ${IMAGE_DIR}/boot)
set(KERNEL_PATH ${KERNEL_DIR}/kernel)
set(ROOT_INCLUDE_DIRS
	${CMAKE_SOURCE_DIR}/include ${CMAKE_BINARY_DIR}/generated/include)
set(KLIBC_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/klibc/include)

# Set compiler flags for kernel code
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -x assembler-with-cpp")
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -fno-pic -fno-ident -ffreestanding -fno-exceptions -nostdlib -nostdinc -fno-stack-protector")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-pic -fno-ident -ffreestanding -fno-exceptions -fno-rtti -nostdlib -nostdinc -fno-stack-protector")

add_subdirectory(arch)
add_subdirectory(kernel)
add_subdirectory(boot)

option(CONFIG_HAVE_TESTS "Enabling this will configure and build tests." OFF)
if (CONFIG_HAVE_TESTS)
	# Reset compiler flags for user mode applications
	set(CMAKE_ASM_FLAGS 	${DEFAULT_CMAKE_ASM_FLAGS})
	set(CMAKE_C_FLAGS 	${DEFAULT_CMAKE_C_FLAGS})
	set(CMAKE_CXX_FLAGS 	${DEFAULT_CMAKE_CXX_FLAGS})

	add_subdirectory(test)
endif()

# Configure config.h.in file.
set(CONFIG_ARCH_MACRO ARCH_${CONFIG_ARCH})
configure_file(${CMAKE_SOURCE_DIR}/include/config.h.in
	${CMAKE_BINARY_DIR}/generated/include/config.h)
