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
    void write(int value);
    void write(unsigned int value);
    void write(long value);
    void write(unsigned long value);
    void write(long long value);
    void write(unsigned long long value);
    void write(float value, int precision = 6);
    void write(double value, int precision = 6);
    void write_hex(unsigned int value);
    void write_hex(unsigned long long value);
    bool has_char();
    char read();
    void handle_interrupt();

private:
    bool is_transmit_empty();
    void write_dec(unsigned long long value);

private:
    static constexpr uint16_t COM1_PORT = 0x3F8;
    static constexpr int BUFFER_SIZE = 128;
    char buffer[BUFFER_SIZE];
    int head = 0;
    int tail = 0;
};