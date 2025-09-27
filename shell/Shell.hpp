#pragma once
#include <stddef.h>

class Shell 
{
private:
    Shell() = default;

public:
    static Shell& instance();
    void handleInput(char c);

private:
    void executeCommand();
    void clear();
    void help();
    void memory();
    void reboot();
    void shutdown();

private:
    char buffer[128];
    size_t pos;
};