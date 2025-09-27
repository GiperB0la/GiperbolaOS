#include "Pic.hpp"
#include "../io/LowLevel_io.hpp"
#include "../../../drivers/VGA.hpp"
#include "../../../drivers/Keyboard.hpp"


extern "C" void irq0_handler() 
{
    // static int ticks = 0;
    // ticks++;
    // if (ticks % 10 == 0) {
    //     DriverVGA::print(".");
    // }
    pic_send_eoi(0);
}

extern "C" void irq1_handler()
{
     Keyboard::instance().handler();
     pic_send_eoi(1);
}