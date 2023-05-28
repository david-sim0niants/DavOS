#ifndef x86__VGA_TEXT_H__
#define x86__VGA_TEXT_H__

#include <stddef.h>


namespace x86 {

static constexpr size_t VGA_TEXT_COLS = 80, VGA_TEXT_ROWS = 25;
static char *const VGA_TEXT_BUFFER_START = (char *const)0xB8000;
static constexpr size_t VGA_TEXT_BUFFER_SIZE = VGA_TEXT_COLS * VGA_TEXT_ROWS;
static constexpr size_t VGA_TEXT_BUFFER_NBYTES = VGA_TEXT_BUFFER_SIZE * 2;

enum VGATextColor {
	VGA_TEXT_COLOR_BLACK         = 0x0,
	VGA_TEXT_COLOR_BLUE          = 0x1,
	VGA_TEXT_COLOR_GREEN         = 0x2,
	VGA_TEXT_COLOR_CYAN          = 0x3,
	VGA_TEXT_COLOR_RED           = 0x4,
	VGA_TEXT_COLOR_MAGENTA       = 0x5,
	VGA_TEXT_COLOR_BROWN         = 0x6,
	VGA_TEXT_COLOR_LIGHT_GRAY    = 0x7,
	VGA_TEXT_COLOR_DARK_GRAY     = 0x8,
	VGA_TEXT_COLOR_LIGHT_BLUE    = 0x9,
	VGA_TEXT_COLOR_LIGHT_GREEN   = 0xa,
	VGA_TEXT_COLOR_LIGHT_CYAN    = 0xb,
	VGA_TEXT_COLOR_LIGHT_RED     = 0xc,
	VGA_TEXT_COLOR_PINK          = 0xd,
	VGA_TEXT_COLOR_YELLOW        = 0xe,
	VGA_TEXT_COLOR_WHITE         = 0xf,
};

constexpr char vga_text_make_color(VGATextColor fg, VGATextColor bg)
{
	return fg | (bg << 4);
}


class VGAText {
public:
	VGAText() = default;

	char get_color() const;
	void set_color(char color);
	long get_offset() const;
	void set_offset(long offset);

	long putc(char c);
	long puts(const char *str);
	long write_buffer(const char *buf_ptr, size_t buf_len);

private:
	long putc__no_off_check(char c);

	char curr_color =
		vga_text_make_color(VGA_TEXT_COLOR_WHITE, VGA_TEXT_COLOR_BLACK);
	long curr_offset = 0;
};

inline char VGAText::get_color() const
{
	return curr_color;
}

inline void VGAText::set_color(char color)
{
	curr_color = color;
}

inline long VGAText::get_offset() const
{
	return curr_offset;
}

inline void VGAText::set_offset(long offset)
{
	if (offset < 0)
		offset = 0;
	else if (offset >= VGA_TEXT_BUFFER_SIZE)
		offset = VGA_TEXT_BUFFER_SIZE;
	curr_offset = offset;
}

long vga_text_write_buffer(const char *buf_data, long buf_size,
		long offset, char color);
void vga_text_clear();

} // namespace x86


#endif
