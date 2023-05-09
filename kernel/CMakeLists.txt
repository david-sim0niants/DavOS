# Build 'cross-architecture' part of the kernel as a static library.
set(TARGET_NAME kernel_cross)

add_library(${TARGET_NAME} STATIC main.c)
set_target_properties(${TARGET_NAME} PROPERTIES PREFIX "" SUFFIX ".elf")

