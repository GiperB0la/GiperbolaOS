#include "Shell.hpp"
#include "../drivers/VGA.hpp"
#include "../lib/String.hpp"
#include "../arch/x86/io/LowLevel_io.hpp"
#include "../arch/x86/memory/Memory.hpp"


Shell& Shell::instance()
{
    static Shell shell;
    return shell;
}

void Shell::handleInput(char c)
{
    if (c == '\n') {
        buffer[pos] = '\0';
        executeCommand();
        pos = 0;
        VGA::instance().print("\n> ");
    }
    else if (c == '\b') {
        if (pos > 0) {
            pos--;
            VGA::instance().moveCursorBack();
            VGA::instance().print(' ');
            VGA::instance().moveCursorBack();
        }
    }
    else {
        if (pos < sizeof(buffer) - 1) {
            buffer[pos++] = c;
            VGA::instance().print(c);
        }
    }
}

void Shell::executeCommand() 
{
    if (strcmp(buffer, "clear") == 0) {
        clear();
    }
    else if (strcmp(buffer, "cls") == 0) {
        clear();
    }
    else if (strcmp(buffer, "help") == 0) {
        help();
    }
    else if (strcmp(buffer, "memory") == 0) {
        memory();
    }
    else if (strcmp(buffer, "reboot") == 0) {
        reboot();
    }
    else if (strcmp(buffer, "shutdown") == 0) {
        shutdown();
    }
    else {
        VGA::instance().print("\nUnknown command: ");
        VGA::instance().print(buffer);
        VGA::instance().print("\n");
    }
}

void Shell::clear()
{
    VGA::instance().clear();
}

void Shell::help()
{
    struct CommandInfo {
        const char* name;
        const char* desc;
    };

    static const CommandInfo commands[] = {
        { "clear / cls", "Clear the screen" },
        { "help",        "Show this help" },
        { "memory",      "Show memory usage" },
        { "reboot",      "Reboot the system" },
        { "shutdown",    "Power off the system" },
    };

    VGA::instance().print("\nAvailable commands:\n");

    for (auto& cmd : commands) {
        // выравнивание под 12 символов
        VGA::instance().print("  ");
        VGA::instance().print(cmd.name);
        int len = strlen(cmd.name);
        for (int i = len; i < 12; i++) {
            VGA::instance().print(' ');
        }
        VGA::instance().print(" - ");
        VGA::instance().print(cmd.desc);
        VGA::instance().print("\n");
    }
}

void Shell::memory()
{
    uint32_t mb_addr = Memory::instance().getMbAddr(); // передали в kmain
    auto* tag = reinterpret_cast<multiboot_tag*>(mb_addr + 8);

    VGA::instance().print("\n=== Multiboot2 Memory Info ===\n");

    uint64_t total_mem = 0, usable_mem = 0, reserved_mem = 0;

    while (tag->type != 0) {
        if (tag->type == 6) { // Memory map
            auto* mmap_tag = reinterpret_cast<multiboot_tag_mmap*>(tag);
            auto* entry = reinterpret_cast<multiboot_mmap_entry*>(mmap_tag + 1);

            while ((uint8_t*)entry < (uint8_t*)mmap_tag + mmap_tag->size) {
                VGA::instance().print(" Base=");
                VGA::instance().print_hex((uint32_t)entry->addr);
                VGA::instance().print("  Len=");
                VGA::instance().print_hex((uint32_t)entry->len);
                VGA::instance().print("  Type=");

                switch (entry->type) {
                    case 1: VGA::instance().print("Usable"); break;
                    case 2: VGA::instance().print("Reserved"); break;
                    case 3: VGA::instance().print("ACPI Reclaimable"); break;
                    case 4: VGA::instance().print("ACPI NVS"); break;
                    case 5: VGA::instance().print("Bad RAM"); break;
                    default: VGA::instance().print("Unknown"); break;
                }
                VGA::instance().print("\n");

                total_mem += entry->len;
                if (entry->type == 1) usable_mem += entry->len;
                else reserved_mem += entry->len;

                entry = reinterpret_cast<multiboot_mmap_entry*>(
                    (uint8_t*)entry + mmap_tag->entry_size
                );
            }
        }

        tag = reinterpret_cast<multiboot_tag*>(
            (uint8_t*)tag + ((tag->size + 7) & ~7)
        );
    }

    VGA::instance().print("\n=== Totals ===\n");
    VGA::instance().print("Total memory: ");
    VGA::instance().print(total_mem / 1024);
    VGA::instance().print(" KB\n");

    VGA::instance().print("Usable memory: ");
    VGA::instance().print(usable_mem / 1024);
    VGA::instance().print(" KB\n");

    VGA::instance().print("Reserved memory: ");
    VGA::instance().print(reserved_mem / 1024);
    VGA::instance().print(" KB\n");
}

void Shell::reboot()
{
    VGA::instance().print("\nRebooting...\n");
    uint8_t good = 0x02;
    while (good & 0x02) {
        good = port_byte_in(0x64);
    }
    port_byte_out(0x64, 0xFE);
}

void Shell::shutdown()
{
    VGA::instance().print("ACPI power off...\n");

    const uint16_t PM1a_CNT     = 0x400;
    const uint16_t SLP_TYP_S5   = 0x2000;
    const uint16_t SLP_EN       = 1 << 13;

    port_word_out(PM1a_CNT, SLP_TYP_S5 | SLP_EN);

    port_word_out(0x604, 0x2000);
    port_word_out(0xB004, 0x0);

    while (true) {
        asm volatile("hlt");
    }
}