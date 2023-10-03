#ifndef _x86_UTILS_VGA__OSTREAM_H__
#define _x86_UTILS_VGA__OSTREAM_H__

#include <x86/utils/vga/console.h>
#include <kstd/io.h>

namespace x86::utils {

class VGA_OStream : public kstd::OStream {
public:
	void putc(const char c) override;
	void puts(const char *str) override;
	void write(const char *str, size_t len) override;
private:
	VGAConsole console;
};

}

#endif
