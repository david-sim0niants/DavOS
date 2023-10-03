#include <x86/utils/vga/ostream.h>


namespace x86::utils {

void VGA_OStream::putc(const char c)
{
	console.putc(c);
}

void VGA_OStream::puts(const char *str)
{
	console.puts(str);
}

void VGA_OStream::write(const char *str, size_t len)
{
	console.write(str, len);
}

}
