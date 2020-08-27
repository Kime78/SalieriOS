#include "vga.h"

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
	return fg | bg << 4;
}
 
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) uc | (uint16_t) color << 8;
}
 
size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}
 
void terminal_initialize(void) 
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) 0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}
 
void terminal_setcolor(uint8_t color) 
{
	terminal_color = color;
}
 
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}
 
void terminal_putchar(char c)
{
    if (c == '\n')
    {
        terminal_row++;
        terminal_column = 0;
        return;
    }
    terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
    if(++terminal_column == VGA_WIDTH) {
        terminal_column = 0;
        if(++terminal_row == VGA_HEIGHT)
        {
            //move the array up
            for(size_t y = 1; y < VGA_HEIGHT; y++)
            {
                for(size_t x = 0; x < VGA_WIDTH; x++)
                {
                    size_t new_idx = (y - 1) * VGA_WIDTH + x;
                    size_t old_idx = y * VGA_WIDTH + x;
                    terminal_buffer[new_idx] = terminal_buffer[old_idx];
                }
            }
            //prepare a new line
            for (size_t x = 0; x < VGA_WIDTH; x++)
            {
                size_t idx = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
                terminal_buffer[idx] = NULL;
            }
        }
    }
}
 
void terminal_write(const char* data, size_t size) 
{
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}
 
void terminal_writestring(const char* data) 
{
	terminal_write(data, strlen(data));
}

