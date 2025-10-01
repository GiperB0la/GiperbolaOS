#pragma once
#include <stddef.h>

class Shell 
{
private:
    Shell() = default;

public:
    static Shell& instance();
    void handle_input(char c);

private:
    void execute_command();
    void clear();
    void help();
    void memory();
    void reboot();
    void shutdown();

private:
    char buffer_[128];
    size_t pos_;
};