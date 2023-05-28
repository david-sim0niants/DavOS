#include <x86/vga_text.h>
#include <string.h>



namespace x86 {

long VGAText::putc(char c)
{
	if (curr_offset >= VGA_TEXT_BUFFER_SIZE)
		return curr_offset = VGA_TEXT_BUFFER_SIZE;
	return putc__no_off_check(c);
}

long VGAText::puts(const char *str)
{
	char c = 0;
	while ((c = *str++) && curr_offset < VGA_TEXT_BUFFER_SIZE) {
		if (c == '\n') {
			curr_offset += VGA_TEXT_COLS;
			continue;
		}
		else if (c == '\r') {
			long row_i = curr_offset / VGA_TEXT_COLS;
			curr_offset = row_i * VGA_TEXT_COLS;
			continue;
		}
		putc__no_off_check(c);
	}
	return curr_offset;
}

long VGAText::write_buffer(const char *buf_ptr, size_t buf_len)
{
	size_t i;
	for (i = 0; i < buf_len && curr_offset < VGA_TEXT_BUFFER_SIZE; ++i) {
		putc__no_off_check(buf_ptr[i]);
	}
	return curr_offset;
}

long VGAText::putc__no_off_check(char c)
{
	long byte_offset = curr_offset * 2;
	VGA_TEXT_BUFFER_START[byte_offset] = c;
	VGA_TEXT_BUFFER_START[byte_offset + 1] = curr_color;
	return ++curr_offset;
}

long vga_text_write_buffer(const char *buf_data, long buf_size,
		long offset, char color)
{
	if (offset + buf_size > VGA_TEXT_BUFFER_SIZE)
		buf_size = VGA_TEXT_BUFFER_SIZE - offset;

	for (long i = 0, byte_i = offset * 2; i < buf_size; ++i) {
		VGA_TEXT_BUFFER_START[byte_i++] = buf_data[i];
		VGA_TEXT_BUFFER_START[byte_i++] = color;
	}

	return offset + buf_size;
}

void vga_text_clear()
{
	memset(VGA_TEXT_BUFFER_START, 0, VGA_TEXT_BUFFER_NBYTES);
}

} // namespace x86
