if (${CONFIG_ARCH} STREQUAL i386 OR ${CONFIG_ARCH} STREQUAL x86_64)
    set(ARCH_DIR x86)
endif()

add_subdirectory(${ARCH_DIR})
