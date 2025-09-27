#include "Shell.hpp"
#include "../drivers/VGA.hpp"
#include "../lib/String.hpp"


Shell& Shell::instance()
{
    static Shell shell;
    return shell;
}

void Shell::handleInput(char c) 
{
    if (c == '\n') { // Enter
        cmdBuffer[pos] = '\0'; // завершаем строку
        executeCommand();
        pos = 0; // сброс буфера
        VGA::instance().print("\n> "); // новый промпт
    }
    else if (c == '\b') { // Backspace
        if (pos > 0) {
            pos--;
            VGA::instance().print("\b");
        }
    } 
    else {
        if (pos < sizeof(cmdBuffer) - 1) {
            cmdBuffer[pos++] = c;
            VGA::instance().print(c); // эхо на экран
        }
    }
}

void Shell::executeCommand() 
{
    if (strcmp(cmdBuffer, "clear") == 0) {
        VGA::instance().clear();
    } 
    else if (strcmp(cmdBuffer, "help") == 0) {
        VGA::instance().print("\nCommands:\n");
        VGA::instance().print("  clear - clear screen\n");
        VGA::instance().print("  help  - show this help\n");
    } 
    else {
        VGA::instance().print("\nUnknown command: ");
        VGA::instance().print(cmdBuffer);
        VGA::instance().print("\n");
    }
}