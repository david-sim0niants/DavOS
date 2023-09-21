# SECTION_NAME, SECTION_FLAGS, SECTION_LABEL and __BINARY_FILE__ are passed
# as -D options to compiler

.section SECTION_NAME, SECTION_FLAGS, @progbits
SECTION_LABEL:
	.incbin __BINARY_FILE__ # include raw binary bytes
