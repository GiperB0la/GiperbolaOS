#pragma once
#include <stddef.h>
#include <stdint.h>

class Keyboard
{
private:
    Keyboard();

public:
    static Keyboard& instance();

public:
    bool has_key();
    char get_char();
    bool is_key_pressed(uint8_t scancode);
    void handler();

private:
    void init();
    char scancode_char(uint8_t scancode);

private:
    bool keyStates[128];
    char buffer[256];
    int head, tail;
};