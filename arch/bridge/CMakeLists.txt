# Build kernel arch library implementation bridge
#
set(TARGET_NAME kernel_arch_bridge)

add_library(${TARGET_NAME} INTERFACE)
target_sources(${TARGET_NAME} INTERFACE boot/setup.cc kout.cc)
target_include_directories(${TARGET_NAME} INTERFACE ${ARCH_INCLUDE_DIR})
