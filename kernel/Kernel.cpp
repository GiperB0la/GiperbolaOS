#include "../drivers/VGA.hpp"
#include "../lib/String.hpp"
#include "../arch/x86/gdt/GDT.hpp"
#include "../arch/x86/interrupts/IDT.hpp"
#include "../drivers/Keyboard.hpp"
#include "../shell/Shell.hpp"
#include "../drivers/Serial.hpp"
#include "../arch/x86/memory/Memory.hpp"


extern "C" void kmain(uint32_t magic, uint32_t addr)
{
    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
        VGA::instance().print("Invalid multiboot magic: ");
        VGA::instance().print((uint32_t)magic);
        while (true) {}
    }

    VGA::instance();
    VGA::instance().print("Booting kernel...\n");

    GDT::instance();
    VGA::instance().print("GDT initialized\n");

    IDT::instance();
    VGA::instance().print("IDT initialized\n");

    asm volatile("sti");
    VGA::instance().print("Interrupts enabled\n");

    Memory::instance(addr);
    VGA::instance().print("Memory manager initialized\n");

    Keyboard::instance();
    VGA::instance().print("Keyboard activated\n");

    VGA::instance().print("\n> ");
    while (true) {
        if (Keyboard::instance().hasKey()) {
            Shell::instance().handleInput(Keyboard::instance().getChar());
        }

        if (Serial::instance().hasChar()) {
            VGA::instance().print(Serial::instance().read());
        }
    }
}