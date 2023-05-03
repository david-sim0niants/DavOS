#ifndef ARCH__VGA_TEXT_H__
#define ARCH__VGA_TEXT_H__


#define VGA_TEXT_BUFFER_START ((char *)0xB8000)
#define VGA_TEXT_COLS 80
#define VGA_TEXT_ROWS 25
#define VGA_TEXT_BUFFER_SIZE VGA_TEXT_COLS * VGA_TEXT_ROWS
#define VGA_TEXT_BUFFER_NBYTES VGA_TEXT_COLS * VGA_TEXT_ROWS * 2

#define VGA_TEXT_COLOR_BLACK         (0 )
#define VGA_TEXT_COLOR_BLUE          (1 )
#define VGA_TEXT_COLOR_GREEN         (2 )
#define VGA_TEXT_COLOR_CYAN          (3 )
#define VGA_TEXT_COLOR_RED           (4 )
#define VGA_TEXT_COLOR_MAGENTA       (5 )
#define VGA_TEXT_COLOR_BROWN         (6 )
#define VGA_TEXT_COLOR_LIGHT_GRAY    (7 )
#define VGA_TEXT_COLOR_DARK_GRAY     (8 )
#define VGA_TEXT_COLOR_LIGHT_BLUE    (9 )
#define VGA_TEXT_COLOR_LIGHT_GREEN   (10)
#define VGA_TEXT_COLOR_LIGHT_CYAN    (11)
#define VGA_TEXT_COLOR_LIGHT_RED     (12)
#define VGA_TEXT_COLOR_PINK          (13)
#define VGA_TEXT_COLOR_YELLOW        (14)
#define VGA_TEXT_COLOR_WHITE         (15)

#define VGA_TEXT_MAKE_COLOR(fg_color, bg_color) ((fg_color) | ((bg_color) << 4))

struct vga_text {
	char curr_color;
	long curr_offset;
};

static inline void vga_text_init(struct vga_text *vga_text)
{
	vga_text->curr_color =
		VGA_TEXT_MAKE_COLOR(VGA_TEXT_COLOR_WHITE, VGA_TEXT_COLOR_BLACK);
	vga_text->curr_offset = 0;
}

static inline void vga_text_set_color(struct vga_text *vga_text, char color)
{
	vga_text->curr_color = color;
}

static inline void vga_text_set_offset(struct vga_text *vga_text, long offset)
{
	vga_text->curr_offset = offset;
}

long vga_text_writeb(struct vga_text *vga_text, char b);

long vga_text_write(struct vga_text *vga_text,
			const char *buf_data, long buf_size);
long vga_text_write_lc(const char *buf_data, long buf_size,
			long offset, char color);

void vga_text_clear();

void vga_text_print(struct vga_text *vga_text, const char *str);


#endif
