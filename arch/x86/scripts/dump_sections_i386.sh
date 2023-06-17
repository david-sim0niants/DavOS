#!/bin/bash

filename=$1
target_format=$2
cc=$3
objcpy=$4

section_memlayout="$(dirname $0)/section_memlayout.S"

sections=( 	"text 			ax"
		"data 			wa"
		"rodata 		a")

for section in "${sections[@]}"
do
	read -r section_label_name section_flags <<< "$section"

	section_name=.$section_label_name
	new_section_name=$section_name.i386
	new_section_label_name=${section_label_name}_i386_start

	# define raw binary and non-binary object filenames
	section_bin_filename=$filename$section_name.bin
	section_obj_filename=$filename$section_name.o

	# dump current section as a raw binary object file
	$objcpy --dump-section $section_name=$section_bin_filename $filename &&\
	# convert the raw binary section object into the target format and
	# set section name, section flags
	# gcc will compile from an assembly file template, which will include
	# $section_bin_filename raw binary bytes in it
	$cc -c $section_memlayout -o $section_obj_filename -DSECTION_NAME=$new_section_name -DSECTION_LABEL=$new_section_label_name -DSECTION_FLAGS=\"$section_flags\" -D__BINARY_FILE__=\"$section_bin_filename\" &&\
	# remove the temporary raw binary object
	rm $section_bin_filename &&\
	# output object filename if objcopying succeeded, otherwise don't
	echo $section_obj_filename
done

exit 0
