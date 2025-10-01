#include "../drivers/VGA.hpp"
#include "../lib/String.hpp"
#include "../arch/x86/gdt/GDT.hpp"
#include "../arch/x86/interrupts/IDT.hpp"
#include "../drivers/Keyboard.hpp"
#include "../shell/Shell.hpp"
#include "../drivers/Serial.hpp"
#include "../arch/x86/memory/Memory.hpp"

void test_memory() 
{
    VGA::instance().print("\n====== Testing memory allocation... ======\n");

    VGA::instance().print("Free pages before test: ");
    VGA::instance().print((unsigned int)Memory::instance().get_free_pages());
    VGA::instance().print("\n");

    void* p1 = Memory::instance().alloc_page();
    VGA::instance().print("Allocated page: ");
    VGA::instance().print_hex((unsigned int)p1);
    VGA::instance().print("\n");

    void* p2 = Memory::instance().alloc_pages(4);
    VGA::instance().print("Allocated 4 pages starting at: ");
    VGA::instance().print_hex((unsigned int)p2);
    VGA::instance().print("\n");

    Memory::instance().free_page(p1);
    VGA::instance().print("Freed single page\n");

    Memory::instance().free_pages(p2, 4);
    VGA::instance().print("Freed 4 pages\n");

    VGA::instance().print("Free pages now: ");
    VGA::instance().print((unsigned int)Memory::instance().get_free_pages());
    VGA::instance().print("\n==========================================\n");
}

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

    test_memory();

    VGA::instance().print("\n> ");
    while (true) {
        if (Keyboard::instance().has_key()) {
            Shell::instance().handle_input(Keyboard::instance().get_char());
        }

        if (Serial::instance().has_char()) {
            VGA::instance().print(Serial::instance().read());
        }
    }
}