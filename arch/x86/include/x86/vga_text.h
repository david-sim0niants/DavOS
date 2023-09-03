#ifndef _x86__VGA_TEXT_H__
#define _x86__VGA_TEXT_H__

#include <stddef.h>
#include <kstd/type_traits.h>
#include <kstd/enum.h>


namespace x86 {

static constexpr size_t vga_text_cols = 80, vga_text_rows = 25;
static char *const vga_text_buffer_start = (char *const)0xB8000;
static constexpr size_t vga_text_buffer_nr_chars = vga_text_cols * vga_text_rows;
static constexpr size_t vga_text_buffer_size = vga_text_buffer_nr_chars * 2;

struct VGATextColors {
	static constexpr char
	Black 		= 0x0,
	Blue         	= 0x1,
	Green        	= 0x2,
	Cyan         	= 0x3,
	Red           	= 0x4,
	Magenta       	= 0x5,
	Brown         	= 0x6,
	LightGray 	= 0x7,
	DarkGray 	= 0x8,
	LightBlue 	= 0x9,
	LightGreen 	= 0xa,
	LightCyan 	= 0xb,
	LightRed 	= 0xc,
	Pink 		= 0xd,
	Yellow        	= 0xe,
	White         	= 0xf;
};


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

	char curr_color = make_color(VGATextColors::White, VGATextColors::Black);
	long curr_offset = 0;

public:
	static constexpr char make_color(char fg, char bg);
	static long write_buffer(const char *buf_data, long buf_size,
		long offset, char color);
	static void clear_screen();
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
	else if (offset >= vga_text_buffer_nr_chars)
		offset = vga_text_buffer_nr_chars;
	curr_offset = offset;
}

constexpr char VGAText::make_color(char fg, char bg)
{
	return fg | (bg << 4);
}

} // namespace x86


#endif
