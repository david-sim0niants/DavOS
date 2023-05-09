#!/bin/bash

filename=$1
target_format=$2

sections=( 	".multiboot_header 	alloc,load,readonly"
		".text 			alloc,load,code,readonly"
		".data 			alloc,load"
		".rodata 		alloc,load,readonly")

for section in "${sections[@]}"
do
	read -r section_name section_flags <<< "$section"
	# define raw binary and non-binary object filenames
	section_bin_filename=$filename$section_name.bin
	section_obj_filename=$filename$section_name.o

	# dump current section as a raw binary object file
	objcopy --dump-section $section_name=$section_bin_filename $filename &&\
	# convert the raw binary section object into the target format
	# and reset the section name and flags
	objcopy -I binary -O $target_format --rename-section .data=$section_name.i386 --set-section-flags .data=$section_flags $section_bin_filename $section_obj_filename && \
	# remove the temporary raw binary object
	rm $section_bin_filename &&\
	# output object filename if objcopying succeeded, otherwise don't
	echo $section_obj_filename
done

exit 0
