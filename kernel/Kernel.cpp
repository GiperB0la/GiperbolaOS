#include "../drivers/VGA.hpp"
#include "../lib/String.hpp"
#include "../arch/x86/gdt/GDT.hpp"
#include "../arch/x86/interrupts/IDT.hpp"
#include "../drivers/Keyboard.hpp"
#include "../shell/Shell.hpp"
#include "../drivers/Serial.hpp"
#include "../arch/x86/memory/Memory.hpp"


extern "C" void kmain(uint32_t magic, uint32_t* addr)
{
    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
        VGA::instance().print("Invalid multiboot magic: ");
        VGA::instance().print((uint32_t)magic);
        while (true) {}
    }

    VGA::instance().print("Booting kernel...\n\n");

    GDT::instance();
    VGA::instance().print_status("GDT initialized", Status::OK);
    Serial::instance().write("[ OK ] GDT initialized\n");

    IDT::instance();
    VGA::instance().print_status("IDT initialized", Status::OK);
    Serial::instance().write("[ OK ] IDT initialized\n");

    asm volatile("sti");
    VGA::instance().print_status("Interrupts enabled", Status::OK);
    Serial::instance().write("[ OK ] Interrupts enabled\n");

    Memory::instance(addr);
    VGA::instance().print_status("Memory manager initialized", Status::OK);
    Serial::instance().write("[ OK ] Memory manager initialized\n");

    Keyboard::instance();
    VGA::instance().print_status("Keyboard activated", Status::OK);
    Serial::instance().write("[ OK ] Keyboard activated\n");

    VGA::instance().print("\n> ");
    while (true) {
        if (Keyboard::instance().has_key()) {
            Shell::instance().handleInput(Keyboard::instance().get_char());
        }

        if (Serial::instance().has_char()) {
            VGA::instance().print(Serial::instance().read());
        }
    }
}