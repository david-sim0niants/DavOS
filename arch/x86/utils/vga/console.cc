#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <x86/utils/vga/console.h>

#include <kstd/algorithm.h>


namespace x86::utils {

VGAConsole::VGAConsole()
{
	reset();
}

void VGAConsole::putc(const char c)
{
	switch (c) {
	case '\n':
		put_lf();
		put_cr();
		return;
	case '\r':
		put_cr();
		return;
	case '\t':
		put_tb();
		return;
	case '\b':
		put_bs();
		return;
	case '\033': {
		if (mode == Mode::Normal)
			mode = Mode::Escape;
		return;
	}
	case '[':
		if (mode == Mode::Escape) {
			mode = Mode::EscapeSequence;
			return;
		} else {
			mode = Mode::Normal;
			break;
		}
	default:
		break;
	}

	if (mode == Mode::Normal) {
		if (c > 0x1F && c != 0x7F)
			put_normal_char(c);
	} else if (mode == Mode::Escape) {
		if (is_esc_exiting_char(c)) {
			process_esc(get_esc_exiting_type(c));
			mode = Mode::Normal;
		}
	} else if (mode == Mode::EscapeSequence) {
		if (is_escseq_exiting_char(c) || seq_idx >= max_seq_len) {
			process_escseq(get_escseq_exiting_type(c));
			seq_idx = 0;
			mode = Mode::Normal;
		} else {
			esc_seq[seq_idx++] = c;
		}
	}
}

void VGAConsole::puts(const char *str)
{
	while (char c = *str++)
		putc(c);
}

void VGAConsole::write(const char *str, size_t len)
{
	while (len--)
		putc(*str++);
}

inline void VGAConsole::put_normal_char(char c)
{
	set_char(c);
	move_cursor_right();
}

inline void VGAConsole::put_cr()
{
	set_cursor(0, curr_y);
}

inline void VGAConsole::put_lf()
{
	move_cursor_down();
}

inline void VGAConsole::put_tb()
{
	set_cell((curr_cell / tab_stop + 1) * tab_stop);
}

inline void VGAConsole::put_bs()
{
	set_char(' ');
	move_cursor_left();
}

inline void VGAConsole::move_cursor_left()
{
	if (curr_x > 0) {
		--curr_x;
		--curr_cell;
	} else if (curr_y > 0) {
		curr_x = nr_cols - 1;
		--curr_y;
		--curr_cell;
	} else {
		move_screen(-1);
		curr_x = nr_cols - 1;
		curr_cell = nr_cols - 1;
	}
}

inline void VGAConsole::move_cursor_right()
{
	if (curr_x < nr_cols - 1) {
		++curr_x;
		++curr_cell;
	} else if (curr_y < nr_rows - 1) {
		curr_x = 0;
		++curr_y;
		++curr_cell;
	} else {
		move_screen(+1);
		curr_x = 0;
		curr_cell = nr_cols * (nr_rows - 1);
	}
}

inline void VGAConsole::move_cursor_down()
{
	if (curr_y < nr_rows - 1) {
		++curr_y;
		curr_cell += nr_cols;
	} else {
		move_screen(+1);
	}
}

inline void VGAConsole::move_cursor_up()
{
	if (curr_y > 0) {
		--curr_y;
		curr_cell -= nr_cols;
	} else {
		move_screen(-1);
	}
}

void VGAConsole::move_screen(long lines)
{
	if (lines == 0)
		return;
	if (lines >= nr_rows || -lines >= nr_rows) {
		clear_screen();
		return;
	}

	// positive number of lines corresponds to moving the screen up
	// negative number of lines corresponds to moving the screen down

	char *dst, *src; long len;
	long cells_outta_screen = nr_cols * lines;
	long bytes_outta_screen = cells_outta_screen * bytes_per_cell;

	long clear_beg_off; long clear_nr_bytes;

	if (lines > 0) {
		dst = buf_start;
		src = buf_start + bytes_outta_screen;
		len = tot_nr_bytes - bytes_outta_screen;
		clear_beg_off = tot_nr_bytes - bytes_outta_screen;
		clear_nr_bytes = bytes_outta_screen;
	} else {
		dst = buf_start - bytes_outta_screen;
		src = buf_start;
		len = tot_nr_bytes + bytes_outta_screen;
		clear_beg_off = 0;
		clear_nr_bytes = -bytes_outta_screen;
	}

	memmove(dst, src, len); // move characters
	memset(buf_start + clear_beg_off, 0, clear_nr_bytes); // clear what was left
}

inline void VGAConsole::clear_screen()
{
	memset(buf_start, 0, tot_nr_bytes);
}

inline void VGAConsole::reset()
{
	clear_screen();
	set_cursor(0, 0);
}

void VGAConsole::set_cursor(long x, long y)
{
	x = kstd::clamp(x, (long)0, nr_cols - 1);
	if (y < 0) {
		move_screen(-y);
		y = 0;
	} else if (y >= nr_rows) {
		move_screen(nr_rows - 1 - y);
		y = nr_rows - 1;
	}

	curr_x = x; curr_y = y;
	curr_cell = y * nr_cols + x;
}

void VGAConsole::set_cell(long cell)
{
	long x = cell % nr_cols;
	long y = cell / nr_cols;

	if (y < 0) {
		move_screen(-y);
		y = 0;
		cell = x;
	} else if (y >= nr_rows) {
		move_screen(nr_rows - 1 - y);
		y = nr_rows - 1;
		cell = (nr_rows - 1) * nr_cols + x;
	}

	curr_x = x; curr_y = y;
	curr_cell = cell;
}

inline void VGAConsole::set_char(char c)
{
	long bytes_off = curr_cell * bytes_per_cell;
	buf_start[bytes_off] = c;
	buf_start[bytes_off + 1] = curr_color;
}

/* Check if the char is a valid escape command exiting char. */
inline bool VGAConsole::is_esc_exiting_char(char c)
{
	switch (c) {
	case (char)EscType::ResetScreen:
		return true;
	default:
		return false;
	}
}

/* Get the type of escape command based on exiting character. */
inline VGAConsole::EscType VGAConsole::get_esc_exiting_type(char c)
{
	return EscType{c};
}

/* Check if the char is a valid escape sequence command exiting char. */
inline bool VGAConsole::is_escseq_exiting_char(char c)
{
	switch (c) {
	case (char)EscSeqType::ColorSetting:
		return true;
	default:
		return false;
	}
}

/* Get the type of escape sequence command based on exiting character. */
inline VGAConsole::EscSeqType VGAConsole::get_escseq_exiting_type(char c)
{
	return EscSeqType{c};
}

inline bool VGAConsole::is_seq_a_num() const
{
	for (char i = 0; i < seq_idx; ++i) {
		if (!isdigit(esc_seq[i])) {
			return false;
		}
	}
	return true;
}

inline int VGAConsole::get_seq_as_num()
{
	esc_seq[seq_idx] = 0;
	return atoi(esc_seq);
}


/* Process escape command. */
void VGAConsole::process_esc(EscType type)
{
	put_normal_char('J');
	switch (type) {
	case EscType::ResetScreen:
		process_esc_reset_screen();
	}
}

/* Process escape 'reset' command. */
void VGAConsole::process_esc_reset_screen()
{
	reset();
}

/* Process escape sequence command. */
void VGAConsole::process_escseq(EscSeqType type)
{
	switch (type) {
	case EscSeqType::ColorSetting:
		process_escseq_color();
	}
}

/* Process escape sequence 'color set' command. */
void VGAConsole::process_escseq_color()
{
	if (!is_seq_a_num())
		return;

	int number = get_seq_as_num();
	if (number < 0)
		return;

	if (number == 0) {
		curr_color = vga_make_color(vga_colors::white, vga_colors::black);
	} else if (number <= 16) {
		char fg_color = number - 1;
		curr_color = vga_make_color(fg_color, curr_color >> 4);
	} else if (number <= 32) {
		char bg_color = (number - 16) - 1;
		curr_color = vga_make_color(curr_color & 0xF, bg_color);
	}
}

}
