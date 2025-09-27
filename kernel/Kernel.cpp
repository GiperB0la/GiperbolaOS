#include "../drivers/VGA.hpp"
#include "../lib/String.hpp"
#include "../arch/x86/gdt/GDT.hpp"
#include "../arch/x86/interrupts/IDT.hpp"
#include "../drivers/Keyboard.hpp"
#include "../shell/Shell.hpp"

extern "C" void test_div0() {
    asm volatile (
        "movl $1, %eax\n"  // делимое = 1
        "xorl %edx, %edx\n" // старшие биты = 0
        "movl $0, %ecx\n"  // делитель = 0
        "divl %ecx\n"      // EAX = EDX:EAX / ECX → вызовет #DE
    );
}

extern "C" void kmain() 
{
    VGA::instance();
    VGA::instance().print("Booting kernel...\n");

    GDT::instance();
    VGA::instance().print("GDT initialized\n");

    IDT::instance();
    VGA::instance().print("IDT initialized\n");

    asm volatile("sti");
    VGA::instance().print("Interrupts enabled\n");

    Keyboard::instance();
    VGA::instance().print("Keyboard activated\n");

    VGA::instance().print("> ");
    while (true) {
        if (Keyboard::instance().hasKey()) {
            Shell::instance().handleInput(Keyboard::instance().getChar());
        }
    }
}