#include <x86/vga_text.h>
#include <string.h>


long vga_text_writeb(struct VGAText *vga_text, char b)
{
	long byte_offset = vga_text->curr_offset * 2;
	VGA_TEXT_BUFFER_START[byte_offset] = b;
	VGA_TEXT_BUFFER_START[byte_offset + 1] = vga_text->curr_color;
	return ++vga_text->curr_offset;
}

long vga_text_write(struct VGAText *vga_text,
			const char *buf_data, long buf_size)
{
	vga_text->curr_offset =
		vga_text_write_lc(buf_data, buf_size,
				vga_text->curr_offset, vga_text->curr_color);
	return vga_text->curr_offset;
}

long vga_text_write_lc(const char *buf_data, long buf_size,
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

void vga_text_print(struct VGAText *vga_text, const char *str)
{
	char c = 0;
	while ((c = *(str++)) && vga_text->curr_offset < VGA_TEXT_BUFFER_SIZE) {
		if (c == '\n') {
			vga_text->curr_offset += VGA_TEXT_COLS;
			continue;
		}
		else if (c == '\r') {
			long row_i = (vga_text->curr_offset / VGA_TEXT_COLS);
			vga_text->curr_offset = row_i * VGA_TEXT_COLS;
			continue;
		}
		vga_text_writeb(vga_text, c);
	}
}
