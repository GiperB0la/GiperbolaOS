#include "../drivers/VGA.hpp"
#include "Panic.hpp"


namespace Kernel 
{
    [[noreturn]] void panic(const char* msg) 
    {
        VGA::instance().set_color(VGA::vga_color(VGA::Color::Red, VGA::Color::Black));
        VGA::instance().print("\n\nKERNEL PANIC: ");
        VGA::instance().print(msg);
        VGA::instance().print("\nSystem halted.\n");

        while (true) {
            asm volatile("cli; hlt"); 
        }
    }
}