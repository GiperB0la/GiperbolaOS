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
    head_ = tail_ = 0;
}

bool Keyboard::has_key() 
{
    return head_ != tail_;
}

char Keyboard::get_char() 
{
    if (head_ == tail_) return 0;
    char c = buffer_[tail_];
    tail_ = (tail_ + 1) % sizeof(buffer_);
    return c;
}

bool Keyboard::is_key_pressed(uint8_t scancode) 
{
    return key_states_[scancode];
}

void Keyboard::handler()
{
    uint8_t scancode = port_byte_in(0x60);

    if (scancode & 0x80) {
        scancode &= 0x7F;
        key_states_[scancode] = false;
    } 
    else {
        key_states_[scancode] = true;
        char c = scancode_char(scancode);
        if (c) {
            buffer_[head_] = c;
            head_ = (head_ + 1) % sizeof(buffer_);
        }
    }
}

char Keyboard::scancode_char(uint8_t scancode) 
{
    if (scancode < 128)
        return scancode_map[scancode];
    return 0;
}