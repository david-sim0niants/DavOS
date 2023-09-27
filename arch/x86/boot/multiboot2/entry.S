.code32

.section .multiboot2_header

.equ MAGIC, 0xE85250D6
.equ ARCHITECTURE, 0
.equ HEADER_LENGTH, .Lheader_end - .Lheader_start
.equ CHECKSUM, -(MAGIC + ARCHITECTURE + HEADER_LENGTH)

.equ OPTIONAL_TAG_FLAG, 1

.equ MULTIBOOT2_MAGIC, 0x36d76289

.Lheader_start:
	.align 4
	.long MAGIC
	.long ARCHITECTURE
	.long HEADER_LENGTH
	.long CHECKSUM

	.align 8
	.short 0 /* terminating tag type */
	.short 0 /* terminating tag flags */
	.long  8 /* terminating tag size */
.Lheader_end:


.section .text_entry, "ax", @progbits

#include <config.h>
#include <ldsym.h>
#include <x86/i386/entry.h>

.globl x86_setup

.type _start, @function
.globl _start
_start:
	cli # disable all interrupts

	cmp $MULTIBOOT2_MAGIC, %eax # compare EAX to multiboot2 magic
	jne .L_start_no_mb2 # jump to failure label if not equal

	mov (__ldsym__stack_top), %esp # setup stack pointer

	push %ebx # EBX stores pointer to multiboot2 info, pass it as an argument
	call x86_setup
	test %eax, %eax
	jnz .L_start_x86_setup_failed # jump to failure label, if non-zero value returned

	jmp _x86_i386_start # jump to i386 entry

.L_start_unknown_state:
	mov $errmsg_unknown_state, %esi
	call print_err_hlt
.L_start_x86_setup_failed:
	mov $errmsg_x86_setup_failed, %esi
	call print_err_hlt
.L_start_no_mb2:
	mov $errmsg_no_mb2, %esi
	call print_err_hlt

.section .text

.equ VGA_TEXT_BUFFER, 0xB8000
.equ RED_ON_BLACK, 0x4

# print error in red
print_err:
	mov $VGA_TEXT_BUFFER, %edi
.Lprint_err_loop:
	movb (%esi), %al
	test %al, %al
	jz .Lprint_err_exit
	movb %al, (%edi)
	inc %edi
	movb $RED_ON_BLACK, (%edi)
	inc %edi
	inc %esi
	jmp .Lprint_err_loop
.Lprint_err_exit:
	ret

# print error in red and halt the system
print_err_hlt:
	call print_err
	hlt


#define ERR_HEADER "MULTIBOOT2 ENTRY ERROR: "

.section .rodata
errmsg_no_mb2:
	.asciz ERR_HEADER "Non-multiboot2 start."
errmsg_x86_setup_failed:
	.asciz ERR_HEADER "x86 setup failed."
errmsg_unknown_state:
	.asciz ERR_HEADER "Unknown state, shouldn't have returned from _x86_i386_start."
