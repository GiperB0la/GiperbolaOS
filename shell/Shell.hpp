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

private:
    char cmdBuffer[128];
    size_t pos;
};