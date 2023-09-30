#include <x86/utils/vga/text_mode.h>
#include <string.h>



namespace x86::utils {

long VGAText::putc(char c)
{
	if (curr_offset >= nr_buf_chars)
		return curr_offset = nr_buf_chars;
	return putc__no_off_check(c);
}

long VGAText::puts(const char *str)
{
	char c = 0;
	while ((c = *str++) && curr_offset < nr_buf_chars)
		putc__no_off_check(c);
	return curr_offset;
}

long VGAText::write_buffer(const char *buf_ptr, size_t buf_len)
{
	size_t i;
	for (i = 0; i < buf_len && curr_offset < nr_buf_chars; ++i) {
		putc__no_off_check(buf_ptr[i]);
	}
	return curr_offset;
}

long VGAText::putc__no_off_check(char c)
{
	if (c == '\n') {
		curr_offset += nr_cols;
	} else if (c == '\r') {
		long row_i = curr_offset / nr_cols;
		curr_offset = row_i * nr_cols;
	} else {
		long byte_offset = curr_offset * 2;
		buf_start[byte_offset] = c;
		buf_start[byte_offset + 1] = static_cast<char>(curr_color);
		++curr_offset;
	}
	return curr_offset;
}

long VGAText::write_buffer(const char *buf_data, long buf_size,
		long offset, char color)
{
	if (offset + buf_size > nr_buf_chars)
		buf_size = nr_buf_chars - offset;

	for (long i = 0, byte_i = offset * 2; i < buf_size; ++i) {
		buf_start[byte_i++] = buf_data[i];
		buf_start[byte_i++] = color;
	}

	return offset + buf_size;
}

void VGAText::clear_screen()
{
	memset(buf_start, 0, nr_buf_bytes);
}

} // namespace x86
