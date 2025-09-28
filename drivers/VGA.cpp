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

    update_cursor();
}

void VGA::print(const char* str)
{
    while (*str) {
        print(*str++);
    }
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

void VGA::print(float value, int precision)
{
    if (value < 0) {
        print('-');
        value = -value;
    }

    unsigned long long int_part = (unsigned long long)value;
    print_dec(int_part);

    print('.');

    float frac = value - (float)int_part;
    for (int i = 0; i < precision; i++) {
        frac *= 10.0f;
        int digit = (int)frac;
        print((char)('0' + digit));
        frac -= digit;
    }
}

void VGA::print(double value, int precision)
{
    if (value < 0) {
        print('-');
        value = -value;
    }

    unsigned long long int_part = (unsigned long long)value;
    print_dec(int_part);

    print('.');

    double frac = value - (double)int_part;
    for (int i = 0; i < precision; i++) {
        frac *= 10.0;
        int digit = (int)frac;
        print((char)('0' + digit));
        frac -= digit;
    }
}

void VGA::print_hex(unsigned int value)
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
    char buffer[19];
    buffer[0] = '0';
    buffer[1] = 'x';

    for (int i = 0; i < 16; i++) {
        buffer[17 - i] = hex_digits[value & 0xF];
        value >>= 4;
    }
    buffer[18] = '\0';

    print(buffer);
}

void VGA::print_status(const char* msg, Status status) 
{
    set_color(vga_color(Color::LightGray, Color::Black));
    print(msg);
    print(" ... ");

    switch (status) {
        case Status::OK:
            set_color(vga_color(Color::LightGreen, Color::Black));
            print("[ OK ]\n");
            break;
        case Status::FAIL:
            set_color(vga_color(Color::LightRed, Color::Black));
            print("[FAIL]\n");
            break;
        case Status::INFO:
            set_color(vga_color(Color::LightCyan, Color::Black));
            print("[INFO]\n");
            break;
    }

    set_color(vga_color(Color::LightGray, Color::Black));
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
    update_cursor();
}

void VGA::set_color(uint8_t color)
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

void VGA::update_cursor()
{
    uint16_t pos = terminal_row * VGA_WIDTH + terminal_col;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void VGA::move_cursor_back()
{
    if (terminal_col > 0) {
        terminal_col--;
    }
    else if (terminal_row > 0) {
        terminal_row--;
        terminal_col = VGA_WIDTH - 1;
    }
    update_cursor();
}