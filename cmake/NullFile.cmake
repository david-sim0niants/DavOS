macro(get_null_file EXTENSION)
	set(NULL_FILE ${CMAKE_BINARY_DIR}/.null.${EXTENSION})
	file(WRITE ${NULL_FILE} "")
endmacro()
