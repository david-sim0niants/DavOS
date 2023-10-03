#ifndef _x86_UTILS_VGA__TEXT_MODE_H__
#define _x86_UTILS_VGA__TEXT_MODE_H__

#include <stddef.h>
#include <string.h>
#include <kstd/type_traits.h>
#include <kstd/algorithm.h>


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
	void get_cursor(long &x, long &y) const;
	void set_cursor(long x, long y);

	char get_char() const; /* Get character under current cursor/offset. */
	void set_char(char c); /* Set character under current cursor/offset. */
	char get_char_color() const; /* Get character color under current cursor/offset.*/
	void set_char_color(char color); /* Set character color under current cursor/offset. */

	void inc_offset() /* Efficiently increment offset. */;
	void dec_offset() /* Efficiently decrement offset. */;

	long putc(char c); /* Put single character. Increments the offset. */
	long puts(const char *str);
	long write(const char *buf_ptr, size_t buf_len);

private:
	long putc__no_off_check(char c);

	char curr_color = make_color(vga_colors::white, vga_colors::black);
	/* Current character offset, related with (curr_x, curr_y) cursor.*/
	long curr_offset = 0;
	long curr_x = 0, curr_y = 0;

public:
	static constexpr char make_color(char fg, char bg);
	static long write_buffer(const char *buf_data, long buf_size,
		long offset, char color);
	static void clear_screen();

	/* VGA text mode buffer start. */
	inline static char *const buf_start = (char *const)0xB8000;
	static constexpr long nr_cols = 80, nr_rows = 25;
	/* Total number of characters in buffer. */
	static constexpr long nr_buf_chars = nr_cols * nr_rows;
	/* Total number of bytes in buffer including characters and colors. */
	static constexpr long nr_buf_bytes = nr_buf_chars * 2;
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

inline void VGAText::get_cursor(long &x, long &y) const
{
	x = curr_x; y = curr_y;
}

inline char VGAText::get_char() const
{
	return buf_start[curr_offset * 2];
}

inline void VGAText::set_char(char c)
{
	long byte_offset = curr_offset * 2;
	buf_start[byte_offset] = c;
	buf_start[byte_offset + 1] = curr_color;
}

inline char VGAText::get_char_color() const
{
	return buf_start[curr_offset * 2 + 1];
}

inline void VGAText::set_char_color(char color)
{
	buf_start[curr_offset * 2 + 1] = color;
}

inline void VGAText::inc_offset()
{
	if (curr_x < nr_cols - 1) [[likely]] {
		++curr_x;
	} else if (curr_y < nr_rows - 1) {
		++curr_y;
		curr_x = nr_cols - 1;
	} else {
		return;
	}
	++curr_offset;
}

inline void VGAText::dec_offset()
{
	if (curr_x > 0) [[likely]] {
		--curr_x;
	} else if (curr_y > 0) {
		--curr_y;
		curr_x = nr_cols - 1;
	} else {
		return;
	}
	--curr_offset;
}

constexpr char VGAText::make_color(char fg, char bg)
{
	return fg | (bg << 4);
}

inline void VGAText::clear_screen()
{
	memset(buf_start, 0, nr_buf_bytes);
}

} // namespace x86::utils


#endif
