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
    bool hasKey();
    char getChar();
    bool isKeyPressed(uint8_t scancode);
    void handler();

private:
    void init();
    char scancodeToChar(uint8_t scancode);

private:
    bool keyStates[128];
    char buffer[256];
    int head, tail;
};