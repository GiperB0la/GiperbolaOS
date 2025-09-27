#include "Keyboard.hpp"
#include "VGA.hpp"
#include "../arch/x86/io/LowLevel_io.hpp"


static const char scancode_map[128] = 
{
    0,  27,'1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`',  0, '\\',
    'z','x','c','v','b','n','m',',','.','/', 0,   '*', 0, ' ',
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

Keyboard::Keyboard()
{
    init();
}

Keyboard& Keyboard::instance()
{
    static Keyboard keyboard;
    return keyboard;
}

void Keyboard::init() 
{
    head = tail = 0;
}

bool Keyboard::hasKey() 
{
    return head != tail;
}

char Keyboard::getChar() 
{
    if (head == tail) return 0;
    char c = buffer[tail];
    tail = (tail + 1) % sizeof(buffer);
    return c;
}

bool Keyboard::isKeyPressed(uint8_t scancode) 
{
    return keyStates[scancode];
}

void Keyboard::handler()
{
    uint8_t scancode = port_byte_in(0x60);

    if (scancode & 0x80) {
        // Key released
        scancode &= 0x7F;
        keyStates[scancode] = false;
    } 
    else {
        // Key pressed
        keyStates[scancode] = true;
        char c = scancodeToChar(scancode);
        if (c) {
            buffer[head] = c;
            head = (head + 1) % sizeof(buffer);
        }
    }
}

char Keyboard::scancodeToChar(uint8_t scancode) 
{
    if (scancode < 128)
        return scancode_map[scancode];
    return 0;
}