#pragma once
#include <stddef.h>
#include <stdint.h>
#include "../lib/String.hpp"

class VGA
{
public:
    enum class Color : uint8_t {
        Black        = 0,
        Blue         = 1,
        Green        = 2,
        Cyan         = 3,
        Red          = 4,
        Magenta      = 5,
        Brown        = 6,
        LightGray    = 7,
        DarkGray     = 8,
        LightBlue    = 9,
        LightGreen   = 10,
        LightCyan    = 11,
        LightRed     = 12,
        LightMagenta = 13,
        Yellow       = 14,
        White        = 15
    };

private:
    VGA();

public:
    static VGA& instance();

public:
    void print(char c);
    void print(const char* str);
    void print(const String& str);
    void print(int value);
    void print(unsigned int value);
    void print(long value);
    void print(unsigned long value);
    void print(long long value);
    void print(unsigned long long value);
    void print_hex(uint32_t value);
    void print_hex(unsigned long long value);
    void clear();
    void setColor(uint8_t color);
    void moveCursorBack();
    static uint8_t vga_color(Color fg, Color bg);

private:
    uint16_t vga_entry(char c, uint8_t color);
    void print_dec(unsigned long long value);
    void scroll();
    void updateCursor();

private:
    static constexpr size_t VGA_WIDTH = 80;
    static constexpr size_t VGA_HEIGHT = 25;
    volatile uint16_t* const VGA_MEMORY;
    size_t terminal_row;
    size_t terminal_col;
    uint8_t terminal_color;
};