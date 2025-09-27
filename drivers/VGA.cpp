#include "VGA.hpp"


static inline void outb(uint16_t port, uint8_t val) 
{
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

VGA::VGA()
    : VGA_MEMORY((uint16_t*)0xB8000),
      terminal_row(0),
      terminal_col(0),
      terminal_color(vga_color(Color::LightGray, Color::Black))
{
    clear();
}

VGA& VGA::instance()
{
    static VGA vga;
    return vga;
}

uint16_t VGA::vga_entry(char c, uint8_t color)
{
    return (uint16_t)c | (uint16_t)color << 8;
}

uint8_t VGA::vga_color(Color fg, Color bg)
{
    return (static_cast<uint8_t>(bg) << 4) | static_cast<uint8_t>(fg);
}

void VGA::print(char c)
{
    if (c == '\n') {
        terminal_col = 0;
        terminal_row++;
    } 
    else {
        VGA_MEMORY[terminal_row * VGA_WIDTH + terminal_col] = vga_entry(c, terminal_color);
        terminal_col++;
        if (terminal_col >= VGA_WIDTH) {
            terminal_col = 0;
            terminal_row++;
        }
    }

    if (terminal_row >= VGA_HEIGHT) {
        scroll();
    }

    updateCursor();
}

void VGA::print(const char* str)
{
    while (*str) {
        print(*str++);
    }
}

void VGA::print(const String& str)
{
    const char* p = str.c_str();
    while (*p) {
        print(*p++);
    }
}

void VGA::print(int value)
{
    if (value < 0) {
        print('-');
        print_dec((unsigned long long)(-value));
    } else {
        print_dec((unsigned long long)value);
    }
}

void VGA::print(unsigned int value) 
{
    print_dec((unsigned long long)value);
}

void VGA::print(long value) 
{
    if (value < 0) {
        print('-');
        print_dec((unsigned long long)(-value));
    } else {
        print_dec((unsigned long long)value);
    }
}

void VGA::print(unsigned long value) 
{
    print_dec((unsigned long long)value);
}

void VGA::print(long long value) 
{
    if (value < 0) {
        print('-');
        print_dec((unsigned long long)(-value));
    } else {
        print_dec((unsigned long long)value);
    }
}

void VGA::print(unsigned long long value) 
{
    print_dec(value);
}

void VGA::print_dec(unsigned long long value)
{
    char buffer[21];
    int i = 0;

    if (value == 0) {
        buffer[i++] = '0';
    } 
    else {
        while (value > 0) {
            buffer[i++] = '0' + (value % 10);
            value /= 10;
        }
    }
    buffer[i] = '\0';

    for (int j = 0; j < i / 2; j++) {
        char tmp = buffer[j];
        buffer[j] = buffer[i - j - 1];
        buffer[i - j - 1] = tmp;
    }

    print(buffer);
}

void VGA::print_hex(uint32_t value)
{
    static const char* hex_digits = "0123456789ABCDEF";
    char buffer[11];
    buffer[0] = '0';
    buffer[1] = 'x';

    for (int i = 0; i < 8; i++) {
        buffer[9 - i] = hex_digits[value & 0xF];
        value >>= 4;
    }
    buffer[10] = '\0';

    print(buffer);
}

void VGA::print_hex(unsigned long long value)
{
    static const char* hex_digits = "0123456789ABCDEF";
    char buffer[19]; // "0x" + 16 символов + '\0'
    buffer[0] = '0';
    buffer[1] = 'x';

    for (int i = 0; i < 16; i++) {
        buffer[17 - i] = hex_digits[value & 0xF];
        value >>= 4;
    }
    buffer[18] = '\0';

    print(buffer);
}

void VGA::clear()
{
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            VGA_MEMORY[y * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
        }
    }
    terminal_row = 0;
    terminal_col = 0;
    updateCursor();
}

void VGA::setColor(uint8_t color)
{
    terminal_color = color;
}

void VGA::scroll()
{
    for (size_t y = 1; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            VGA_MEMORY[(y-1) * VGA_WIDTH + x] = VGA_MEMORY[y * VGA_WIDTH + x];
        }
    }
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        VGA_MEMORY[(VGA_HEIGHT-1) * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
    }
    terminal_row = VGA_HEIGHT - 1;
}

void VGA::updateCursor()
{
    uint16_t pos = terminal_row * VGA_WIDTH + terminal_col;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void VGA::moveCursorBack()
{
    if (terminal_col > 0) {
        terminal_col--;
    }
    else if (terminal_row > 0) {
        terminal_row--;
        terminal_col = VGA_WIDTH - 1;
    }
    updateCursor();
}