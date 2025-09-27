#pragma once
#include <stdint.h>

class Serial
{
private:
    Serial();

public:
    static Serial& instance();

public:
    void init();
    void write(char c);
    void write(const char* str);
    // новые методы для приёма
    bool hasChar();
    char read();
    // IRQ обработчик
    void handleInterrupt();

private:
    bool isTransmitEmpty();

private:
    static constexpr uint16_t COM1_PORT = 0x3F8;
    static constexpr int BUFFER_SIZE = 128;
    char buffer[BUFFER_SIZE];
    int head = 0; // куда писать
    int tail = 0; // откуда читать
};