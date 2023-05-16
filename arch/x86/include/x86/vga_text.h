#ifndef x86__VGA_TEXT_H__
#define x86__VGA_TEXT_H__


#define VGA_TEXT_BUFFER_START ((char *)0xB8000)
#define VGA_TEXT_COLS 80
#define VGA_TEXT_ROWS 25
#define VGA_TEXT_BUFFER_SIZE VGA_TEXT_COLS * VGA_TEXT_ROWS
#define VGA_TEXT_BUFFER_NBYTES VGA_TEXT_COLS * VGA_TEXT_ROWS * 2

#define VGA_TEXT_COLOR_BLACK         (0x0)
#define VGA_TEXT_COLOR_BLUE          (0x1)
#define VGA_TEXT_COLOR_GREEN         (0x2)
#define VGA_TEXT_COLOR_CYAN          (0x3)
#define VGA_TEXT_COLOR_RED           (0x4)
#define VGA_TEXT_COLOR_MAGENTA       (0x5)
#define VGA_TEXT_COLOR_BROWN         (0x6)
#define VGA_TEXT_COLOR_LIGHT_GRAY    (0x7)
#define VGA_TEXT_COLOR_DARK_GRAY     (0x8)
#define VGA_TEXT_COLOR_LIGHT_BLUE    (0x9)
#define VGA_TEXT_COLOR_LIGHT_GREEN   (0xa)
#define VGA_TEXT_COLOR_LIGHT_CYAN    (0xb)
#define VGA_TEXT_COLOR_LIGHT_RED     (0xc)
#define VGA_TEXT_COLOR_PINK          (0xd)
#define VGA_TEXT_COLOR_YELLOW        (0xe)
#define VGA_TEXT_COLOR_WHITE         (0xf)

#define VGA_TEXT_MAKE_COLOR(fg_color, bg_color) ((fg_color) | ((bg_color) << 4))

struct VGAText {
	char curr_color;
	long curr_offset;
};

static inline void vga_text_init(struct VGAText *vga_text)
{
	vga_text->curr_color =
		VGA_TEXT_MAKE_COLOR(VGA_TEXT_COLOR_WHITE, VGA_TEXT_COLOR_BLACK);
	vga_text->curr_offset = 0;
}

static inline void vga_text_set_color(struct VGAText *vga_text, char color)
{
	vga_text->curr_color = color;
}

static inline void vga_text_set_offset(struct VGAText *vga_text, long offset)
{
	vga_text->curr_offset = offset;
}

long vga_text_writeb(struct VGAText *vga_text, char b);

long vga_text_write(struct VGAText *vga_text,
			const char *buf_data, long buf_size);
long vga_text_write_lc(const char *buf_data, long buf_size,
			long offset, char color);

void vga_text_clear();

void vga_text_print(struct VGAText *vga_text, const char *str);


#endif
