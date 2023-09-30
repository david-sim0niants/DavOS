#ifndef _x86_UTILS_VGA__CONSOLE_H__
#define _x86_UTILS_VGA__CONSOLE_H__

#include "text_mode.h"

namespace x86::utils {

/* VGA Text based console. */
class VGAConsole {
public:
	VGAConsole();

	/* Put a character. */
	void putc(const char c);
	/* Put a null-terminated string. */
	void puts(const char *str);
	/* Put a string with length. */
	void puts(const char *str, size_t len);

	static constexpr unsigned int tab_stop = 8;
private:
	VGAText vga_text;
	enum Mode {
		Mode_Normal,
		Mode_Escape,
		Mode_EscapeSequence,
	} mode = Mode_Normal;
	char escseq[3] {};
	char seq_idx = 0;

	void put_cr();
	void put_lf();
	void put_tb();
	void put_bs();

	void process_escseq();
};

}

#endif
