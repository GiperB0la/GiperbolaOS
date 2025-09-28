#include "Pic.hpp"
#include "../io/LowLevel_io.hpp"
#include "../../../drivers/VGA.hpp"
#include "../../../drivers/Keyboard.hpp"
#include "../../../drivers/Serial.hpp"


extern "C" void irq0_handler() 
{
    static uint64_t ticks = 0;
    ticks++;

    if (ticks % 100 == 0) {
        // VGA::instance().print(".");
    }

    pic_send_eoi(0);
}

extern "C" void irq1_handler()
{
    Keyboard::instance().handler();
    pic_send_eoi(1);
}

// irq2_handler

extern "C" void irq3_handler()
{
    // COM2 0x2F8
    pic_send_eoi(3);
}

extern "C" void irq4_handler()
{
    Serial::instance().handle_interrupt();
    pic_send_eoi(4);
}