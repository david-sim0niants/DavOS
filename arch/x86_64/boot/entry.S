.code32

.section .multiboot_header

.equ MAGIC, 0xE85250D6 
.equ ARCHITECTURE, 0
.equ HEADER_LENGTH, header_end - header_start
.equ CHECKSUM, -(MAGIC + ARCHITECTURE + HEADER_LENGTH)

.equ OPTIONAL_TAG_FLAG, 1

#ifdef __ELF__
    #define AUTO_KLUDGE, 0
#else
    #define AUTO_KLUDGE, 0x10000
#endif

header_start:
    .align 4
    .long MAGIC
    .long ARCHITECTURE
    .long HEADER_LENGTH
    .long CHECKSUM

    .align 8

#ifdef __ELF__
address_tag_start:
    .short 2 /* type */
    .short OPTIONAL_TAG_FLAG /* flags */
    .long address_tag_end - address_tag_start /* size */
    .long header_start /* header_addr */
    .long entry /* load_addr */
    .long data_end /* load_end_addr */
    .long bss_end /* bss_end_addr */
address_tag_end:

entry_address_tag_start:
    .short 3 /* type */
    .short OPTIONAL_TAG_FLAG
    .long entry_address_tag_end - entry_address_tag_start
    .long entry
entry_address_tag_end:
#endif 

    .short 0 /* terminating tag type */
    .short 0 /* terminating tag flags */
    .long  8 /* terminating tag size */

header_end:


.section .text
entry:

.section .data
data_end:

.section .bss

    .align 16
stack_bottom:
    .skip 1 << 16
stack_top:

bss_end:
