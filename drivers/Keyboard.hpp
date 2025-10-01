#pragma once
#include <stddef.h>
#include <stdint.h>

class Keyboard
{
private:
    Keyboard();
    ~Keyboard() = default;
    Keyboard(const Keyboard&) = delete;
    Keyboard& operator=(const Keyboard&) = delete;

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
    bool key_states_[128];
    char buffer_[256];
    int head_, tail_;
};