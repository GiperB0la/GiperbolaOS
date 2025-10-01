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

void Shell::handle_input(char c)
{
    if (c == '\n') {
        buffer_[pos_] = '\0';
        execute_command();
        pos_ = 0;
        VGA::instance().print("\n> ");
    }
    else if (c == '\b') {
        if (pos_ > 0) {
            pos_--;
            VGA::instance().move_cursor_back();
            VGA::instance().print(' ');
            VGA::instance().move_cursor_back();
        }
    }
    else {
        if (pos_ < sizeof(buffer_) - 1) {
            buffer_[pos_++] = c;
            VGA::instance().print(c);
        }
    }
}

void Shell::execute_command() 
{
    if (strcmp(buffer_, "clear") == 0) {
        clear();
    }
    else if (strcmp(buffer_, "cls") == 0) {
        clear();
    }
    else if (strcmp(buffer_, "help") == 0) {
        help();
    }
    else if (strcmp(buffer_, "memory") == 0) {
        memory();
    }
    else if (strcmp(buffer_, "reboot") == 0) {
        reboot();
    }
    else if (strcmp(buffer_, "shutdown") == 0) {
        shutdown();
    }
    else {
        VGA::instance().print("\nUnknown command: ");
        VGA::instance().print(buffer_);
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
    Memory::instance().memory_info();
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