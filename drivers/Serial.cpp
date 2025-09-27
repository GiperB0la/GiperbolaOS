#include "Serial.hpp"
#include "../arch/x86/io/LowLevel_io.hpp"


Serial::Serial()
{
    init();
}

Serial& Serial::instance()
{
    static Serial serial;
    return serial;
}

void Serial::init() 
{
    port_byte_out(COM1_PORT + 1, 0x00); // запретить прерывания
    port_byte_out(COM1_PORT + 3, 0x80); // включить DLAB
    port_byte_out(COM1_PORT + 0, 0x03); // делитель = 3 (38400 бод)
    port_byte_out(COM1_PORT + 1, 0x00);
    port_byte_out(COM1_PORT + 3, 0x03); // 8N1
    port_byte_out(COM1_PORT + 2, 0xC7); // FIFO
    port_byte_out(COM1_PORT + 4, 0x0B); // IRQs включены, RTS/DSR
    port_byte_out(COM1_PORT + 1, 0x01); // разрешить прерывание при приёме
}

bool Serial::isTransmitEmpty() 
{
    return port_byte_in(COM1_PORT + 5) & 0x20;
}

void Serial::write(char c)
{
    while (!isTransmitEmpty());
    port_byte_out(COM1_PORT, c);
}

void Serial::write(const char* str) 
{
    while (*str) {
        write(*str++);
    }
}

void Serial::handleInterrupt() 
{
    char c = port_byte_in(COM1_PORT);
    int next = (head + 1) % BUFFER_SIZE;
    if (next != tail) { // если буфер не полон
        buffer[head] = c;
        head = next;
    }
}

bool Serial::hasChar()
{
    return head != tail;
}

char Serial::read()
{
    if (head == tail) return 0; // пусто
    char c = buffer[tail];
    tail = (tail + 1) % BUFFER_SIZE;
    return c;
}