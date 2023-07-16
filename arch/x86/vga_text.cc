#include <x86/vga_text.h>
#include <string.h>



namespace x86 {

long VGAText::putc(char c)
{
	if (curr_offset >= vga_text_buffer_nr_chars)
		return curr_offset = vga_text_buffer_nr_chars;
	return putc__no_off_check(c);
}

long VGAText::puts(const char *str)
{
	char c = 0;
	while ((c = *str++) && curr_offset < vga_text_buffer_nr_chars) {
		if (c == '\n') {
			curr_offset += vga_text_cols;
			continue;
		}
		else if (c == '\r') {
			long row_i = curr_offset / vga_text_cols;
			curr_offset = row_i * vga_text_cols;
			continue;
		}
		putc__no_off_check(c);
	}
	return curr_offset;
}

long VGAText::write_buffer(const char *buf_ptr, size_t buf_len)
{
	size_t i;
	for (i = 0; i < buf_len && curr_offset < vga_text_buffer_nr_chars; ++i) {
		putc__no_off_check(buf_ptr[i]);
	}
	return curr_offset;
}

long VGAText::putc__no_off_check(char c)
{
	long byte_offset = curr_offset * 2;
	vga_text_buffer_start[byte_offset] = c;
	vga_text_buffer_start[byte_offset + 1] = static_cast<char>(curr_color);
	return ++curr_offset;
}

long vga_text_write_buffer(const char *buf_data, long buf_size,
		long offset, char color)
{
	if (offset + buf_size > vga_text_buffer_nr_chars)
		buf_size = vga_text_buffer_nr_chars - offset;

	for (long i = 0, byte_i = offset * 2; i < buf_size; ++i) {
		vga_text_buffer_start[byte_i++] = buf_data[i];
		vga_text_buffer_start[byte_i++] = color;
	}

	return offset + buf_size;
}

void vga_text_clear()
{
	memset(vga_text_buffer_start, 0, vga_text_buffer_size);
}

} // namespace x86
