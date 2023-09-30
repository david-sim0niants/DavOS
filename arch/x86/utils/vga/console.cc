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
		if (c == 'm' || seq_idx >= 3) {
			process_escseq();
			seq_idx = 0;
			mode = Mode_Normal;
		} else {
			escseq[seq_idx++] = c;
		}
	}
}

void VGAConsole::puts(const char *str)
{
	while (char c = *str++)
		putc(c);
}

void VGAConsole::puts(const char *str, size_t len)
{
	while (len--) {
		putc(*str++);
	}
}

inline void VGAConsole::put_cr()
{
	long row_i = vga_text.get_offset() / VGAText::nr_cols;
	vga_text.set_offset(row_i * VGAText::nr_cols);
}

inline void VGAConsole::put_lf()
{
	vga_text.set_offset(vga_text.get_offset() + VGAText::nr_cols);
}

inline void VGAConsole::put_tb()
{
	vga_text.set_offset((vga_text.get_offset() / tab_stop + 1) * tab_stop);
}

inline void VGAConsole::put_bs()
{
	vga_text.set_char(' ');
	vga_text.set_offset(vga_text.get_offset() - 1);
}

void VGAConsole::process_escseq()
{
	bool is_number = true;
	for (char i = 0; i < seq_idx; ++i) {
		if (!isdigit(escseq[i])) {
			is_number = false;
			break;
		}
	}

	if (!is_number)
		return;

	escseq[seq_idx] = 0;
	int number = atoi(escseq);

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
