set(TARGET_NAME kernel_x86_mb2_entry)
add_library(${TARGET_NAME} STATIC entry.S multiboot_info.cc)
