.code32

.section .multiboot_header

.equ MAGIC, 0xE85250D6
.equ ARCHITECTURE, 0
.equ HEADER_LENGTH, header_end - header_start
.equ CHECKSUM, -(MAGIC + ARCHITECTURE + HEADER_LENGTH)

.equ OPTIONAL_TAG_FLAG, 1

#ifdef __ELF__
	#define AUTO_KLUDGE 0
#else
	#define AUTO_KLUDGE 0x10000
#endif

header_start:
	.align 4
	.long MAGIC
	.long ARCHITECTURE
	.long HEADER_LENGTH
	.long CHECKSUM

	.align 8

#ifndef __ELF__
address_tag_start:
	.short 2 /* type */
	.short OPTIONAL_TAG_FLAG /* flags */
	.long address_tag_end - address_tag_start /* size */
	.long header_start /* header_addr */
	.long _start /* load_addr */
	.long data_end /* load_end_addr */
	.long bss_end /* bss_end_addr */
address_tag_end:

entry_address_tag_start:
	.short 3 /* type */
	.short OPTIONAL_TAG_FLAG
	.long entry_address_tag_end - entry_address_tag_start
	.long _start
entry_address_tag_end:
#endif

	.short 0 /* terminating tag type */
	.short 0 /* terminating tag flags */
	.long  8 /* terminating tag size */

header_end:


.section .data

.type p_multiboot_info, @object
.globl p_multiboot_info
p_multiboot_info:
	# pointer to multiboot info or NULL if we're not booted via multiboot
	.long 0
data_end:


#include <config.h>
#include "gdt.h"

.section .text

# virtual memory address of the function that starts the ready to go kernel
.globl __ldconfig__KERNEL_TEXT_START_VMA
# initial stack grows below the address where kernel start function will be located
.globl __ldconfig__STACK_TOP

.type early_init, @function
.globl early_init

.type _start, @function
.globl _start
_start:
	cli
	mov (__ldconfig__STACK_TOP), %esp # setup stack pointer
	call get_multiboot_info # get multiboot info
	call early_init # early CPU and kernel initialization
	test %eax, %eax
	jnz .Lstart_fail

	call setup_gdt
.Lload_gdt:
	lgdt (%eax)
	mov $0x10, %ax 	# 0x10 = sizeof(GDT_Entry) * 2, where
			# 2 == data segment selector index
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss
	ljmp $0x8, $.Lnext 	# 0x8 == sizeof(GDT_Entry) * 1, where
				# 1 == code segment selector index
.Lnext:
#if CONFIG_ARCH == ARCH_x86_64
	.code64
	movq __ldconfig__KERNEL_TEXT_START_VMA(%rip), %rax
	callq *%rax # jump onto kernel
	.code32
#else
	jmp *__ldconfig__KERNEL_TEXT_START_VMA # jump onto kernel
#endif

.Lstart_fail:
	hlt


.equ MULTIBOOT2_MAGIC, 0x36d76289

get_multiboot_info:
	cmp $MULTIBOOT2_MAGIC, %eax # compare EAX value with the magic number
	je .Lbooted_via_multiboot # avoid setting EBX value to zero
	xor %ebx, %ebx
.Lbooted_via_multiboot: # assign the pointer or NULL to p_multiboot_info
	mov %ebx, p_multiboot_info
	ret

.section .kernel_start_placeholder, "ax", @progbits
	hlt # placeholder code for the kernel start
