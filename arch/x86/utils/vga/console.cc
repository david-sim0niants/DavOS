#include <x86/utils/vga/console.h>
#include <ctype.h>
#include <stdlib.h>


namespace x86::utils {

VGAConsole::VGAConsole()
{
	vga_text.clear_screen();
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
		if (mode == Mode_Normal)
			mode = Mode_Escape;
		else
			mode = Mode_Normal;
		return;
	}
	case '[':
		if (mode == Mode_Escape) {
			mode = Mode_EscapeSequence;
			return;
		} else {
			mode = Mode_Normal;
			break;
		}
	default:
		break;
	}

	if (mode == Mode_Escape)
		mode = Mode_Normal;

	if (mode == Mode_Normal) {
		if (c > 0x1F && c != 0x7F) { // normal character
			vga_text.putc(c);
		}
	} else if (mode == Mode_EscapeSequence) {
		if (c == 'm' || seq_idx >= max_seq_len) {
			process_escseq();
			seq_idx = 0;
			mode = Mode_Normal;
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

inline void VGAConsole::put_cr()
{
	long row_i = vga_text.get_offset() / VGAText::nr_cols;
	vga_text.set_offset(row_i * VGAText::nr_cols);
}

inline void VGAConsole::put_lf()
{
	long x, y;
	vga_text.get_cursor(x, y);
	vga_text.set_cursor(x, y + 1);
}

inline void VGAConsole::put_tb()
{
	long _, y;
	vga_text.get_cursor(_, y);
	vga_text.set_cursor(0, y);
}

inline void VGAConsole::put_bs()
{
	vga_text.set_char(' ');
	vga_text.dec_offset();
}

void VGAConsole::process_escseq()
{
	bool is_number = true;
	for (char i = 0; i < seq_idx; ++i) {
		if (!isdigit(esc_seq[i])) {
			is_number = false;
			break;
		}
	}

	if (!is_number)
		return;

	esc_seq[seq_idx] = 0;
	int number = atoi(esc_seq);

	if (number < 0)
		return;

	if (number == 0) {
		char color = VGAText::make_color(vga_colors::white, vga_colors::black);
		vga_text.set_color(color);
	} else if (number <= 16) {
		char fg_color = number - 1;
		char color = VGAText::make_color(fg_color, vga_text.get_color() >> 4);
		vga_text.set_color(color);
	} else if (number <= 32) {
		char bg_color = (number - 16) - 1;
		char color = VGAText::make_color(vga_text.get_color() & 0xF, bg_color);
		vga_text.set_color(color);
	}
}

}
