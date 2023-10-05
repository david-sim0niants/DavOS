#ifndef _x86_UTILS_VGA__CONSOLE_H__
#define _x86_UTILS_VGA__CONSOLE_H__

#include <stddef.h>

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

constexpr char vga_make_color(char fg, char bg)
{
	return fg | (bg << 4);
}

/* VGA Text based console. */
class VGAConsole {
private:
	/* Console typing mode. */
	enum class Mode {
		Normal, // Normal character typing
		Escape, // Escape (possibly) command mode
		EscapeSequence, // Escape sequence (possibly) command mode
	};

	/* Escape command type. */
	enum class EscType {
		ResetScreen = 'c',
	};

	/* Escape sequence command type. */
	enum class EscSeqType {
		ColorSetting = 'm',
	};
public:
	VGAConsole();

	/* Put a character. */
	void putc(const char c);
	/* Put a null-terminated string. */
	void puts(const char *str);
	/* Put a string with length. */
	void write(const char *str, size_t len);

	static constexpr unsigned int tab_stop = 8;

private:
	void put_normal_char(char c);
	void put_cr();
	void put_lf();
	void put_tb();
	void put_bs();

	void move_cursor_left();
	void move_cursor_right();
	void move_cursor_down();
	void move_cursor_up();

	void move_screen(long lines);
	void clear_screen();

	void reset();

	void set_cursor(long x, long y);
	void set_cell(long cell);

	void set_char(char c);

	static bool is_esc_exiting_char(char c);
	static EscType get_esc_exiting_type(char c);

	static bool is_escseq_exiting_char(char c);
	static EscSeqType get_escseq_exiting_type(char c);
	bool is_seq_a_num() const;
	int get_seq_as_num();

	void process_esc(EscType type);
	void process_esc_reset_screen();

	void process_escseq(EscSeqType type);
	void process_escseq_color();

	/* VGA text mode buffer start. */
	inline static char *const buf_start = (char *)0xB8000;
	static constexpr long nr_cols = 80, nr_rows = 25;
	/* Total number of cells in buffer. */
	static constexpr long tot_nr_cells = nr_cols * nr_rows;
	/* Number of bytes per cell. */
	static constexpr long bytes_per_cell = 2;
	/* Total number of bytes in buffer including characters and colors. */
	static constexpr long tot_nr_bytes = tot_nr_cells * bytes_per_cell;

	long curr_x = 0; /* Current x compontent of the cursor. */
	long curr_y = 0; /* Current y compontent of the cursor. */
	long curr_cell = 0; /* Current cell number of the cursor. */
	char curr_color = vga_make_color(vga_colors::white, vga_colors::black);

	Mode mode = Mode::Normal;
	static constexpr unsigned int max_seq_len = 3;
	char esc_seq[max_seq_len] {};
	char seq_idx = 0;
};

}

#endif
