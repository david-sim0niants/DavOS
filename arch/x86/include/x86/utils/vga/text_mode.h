#ifndef _x86_UTILS_VGA__TEXT_MODE_H__
#define _x86_UTILS_VGA__TEXT_MODE_H__

#include <stddef.h>
#include <kstd/type_traits.h>
#include <kstd/enum.h>


namespace x86::utils {

namespace vga_colors {
	static constexpr char
	black 		= 0x0,
	blue         	= 0x1,
	green        	= 0x2,
	cyan         	= 0x3,
	red           	= 0x4,
	magenta       	= 0x5,
	brown         	= 0x6,
	light_gray 	= 0x7,
	dark_gray 	= 0x8,
	light_blue 	= 0x9,
	light_green 	= 0xa,
	light_cyan 	= 0xb,
	light_red 	= 0xc,
	pink 		= 0xd,
	yellow        	= 0xe,
	white         	= 0xf;
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

	char curr_color = make_color(vga_colors::white, vga_colors::black);
	long curr_offset = 0;

public:
	static constexpr char make_color(char fg, char bg);
	static long write_buffer(const char *buf_data, long buf_size,
		long offset, char color);
	static void clear_screen();

	inline static char *const buf_start = (char *const)0xB8000;
	static constexpr size_t nr_cols = 80, nr_rows = 25;
	static constexpr size_t nr_buf_chars = nr_cols * nr_rows;
	static constexpr size_t nr_buf_bytes = nr_buf_chars * 2;
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
	else if (offset >= nr_buf_chars)
		offset = nr_buf_chars;
	curr_offset = offset;
}

constexpr char VGAText::make_color(char fg, char bg)
{
	return fg | (bg << 4);
}

} // namespace x86::utils


#endif
