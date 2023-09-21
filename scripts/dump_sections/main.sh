#!/bin/bash

filename=$1
o_sect_prefix=$2
o_fn_prefix=$3
bitness=$4
cc=$5
objcpy=$6

section_memlayout="$(dirname $0)/section_memlayout.S"

sections=( 	"text 			ax"
		"data 			wa"
		"rodata 		a")

for section in "${sections[@]}"
do
	read -r section_label_name section_flags <<< "$section"

	section_name=.$section_label_name
	new_section_name=.$o_sect_prefix$section_name
	new_section_label_name=${o_sect_prefix}_${section_label_name}_start

	# define raw binary and non-binary object filenames
	section_bin_filename=$o_fn_prefix$section_name.bin
	section_obj_filename=$o_fn_prefix$section_name.o

	dump_section_comm="$objcpy --dump-section $section_name=$section_bin_filename $filename"
	# dump current section as a raw binary object file
	if [[ $($dump_section_comm 2>&1 >/dev/null) || $? -ne 0 ]]; then
		# Assume a non-empty output is an error and create an empty file
		truncate $section_bin_filename --size 0
	fi

	# convert the raw binary section object into the target format and
	# set section name, section flags
	# gcc will compile from an assembly file template, which will include
	# $section_bin_filename raw binary bytes in it
	$cc -m$bitness -c $section_memlayout -o $section_obj_filename -DSECTION_NAME=$new_section_name -DSECTION_LABEL=$new_section_label_name -DSECTION_FLAGS=\"$section_flags\" -D__BINARY_FILE__=\"$section_bin_filename\" -U$o_sect_prefix &&\
	# remove the temporary raw binary object
	rm $section_bin_filename
done

exit 0
