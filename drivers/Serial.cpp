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
    port_byte_out(COM1_PORT + 1, 0x00);
    port_byte_out(COM1_PORT + 3, 0x80);
    port_byte_out(COM1_PORT + 0, 0x03);
    port_byte_out(COM1_PORT + 1, 0x00);
    port_byte_out(COM1_PORT + 3, 0x03);
    port_byte_out(COM1_PORT + 2, 0xC7);
    port_byte_out(COM1_PORT + 4, 0x0B);
    port_byte_out(COM1_PORT + 1, 0x01);
}

bool Serial::is_transmit_empty() 
{
    return port_byte_in(COM1_PORT + 5) & 0x20;
}

void Serial::write(char c)
{
    while (!is_transmit_empty());
    port_byte_out(COM1_PORT, c);
}

void Serial::write(const char* str) 
{
    while (*str) {
        write(*str++);
    }
}

void Serial::write_dec(unsigned long long value) 
{
    char buffer[21];
    int i = 0;

    if (value == 0) {
        buffer[i++] = '0';
    } 
    else {
        while (value > 0) {
            buffer[i++] = '0' + (value % 10);
            value /= 10;
        }
    }
    buffer[i] = '\0';

    for (int j = 0; j < i / 2; j++) {
        char tmp = buffer[j];
        buffer[j] = buffer[i - j - 1];
        buffer[i - j - 1] = tmp;
    }

    write(buffer);
}

void Serial::write(int value) 
{
    if (value < 0) {
        write('-');
        write_dec((unsigned long long)(-value));
    } 
    else {
        write_dec((unsigned long long)value);
    }
}

void Serial::write(unsigned int value) 
{
    write_dec((unsigned long long)value);
}

void Serial::write(long value) 
{
    if (value < 0) {
        write('-');
        write_dec((unsigned long long)(-value));
    } 
    else {
        write_dec((unsigned long long)value);
    }
}

void Serial::write(unsigned long value) 
{
    write_dec((unsigned long long)value);
}

void Serial::write(long long value) 
{
    if (value < 0) {
        write('-');
        write_dec((unsigned long long)(-value));
    } 
    else {
        write_dec((unsigned long long)value);
    }
}

void Serial::write(unsigned long long value) 
{
    write_dec(value);
}

void Serial::write(float value, int precision)
{
    if (value < 0) {
        write('-');
        value = -value;
    }

    unsigned long long int_part = (unsigned long long)value;
    write_dec(int_part);

    write('.');

    float frac = value - (float)int_part;
    for (int i = 0; i < precision; i++) {
        frac *= 10.0f;
        int digit = (int)frac;
        write((char)('0' + digit));
        frac -= digit;
    }
}

void Serial::write(double value, int precision) 
{
    if (value < 0) {
        write('-');
        value = -value;
    }

    unsigned long long int_part = (unsigned long long)value;
    write_dec(int_part);

    write('.');

    double frac = value - (double)int_part;
    for (int i = 0; i < precision; i++) {
        frac *= 10.0;
        int digit = (int)frac;
        write((char)('0' + digit));
        frac -= digit;
    }
}

void Serial::write_hex(unsigned int value) 
{
    static const char* hex_digits = "0123456789ABCDEF";
    char buffer[11];
    buffer[0] = '0';
    buffer[1] = 'x';

    for (int i = 0; i < 8; i++) {
        buffer[9 - i] = hex_digits[value & 0xF];
        value >>= 4;
    }
    buffer[10] = '\0';

    write(buffer);
}

void Serial::write_hex(unsigned long long value) 
{
    static const char* hex_digits = "0123456789ABCDEF";
    char buffer[19];
    buffer[0] = '0';
    buffer[1] = 'x';

    for (int i = 0; i < 16; i++) {
        buffer[17 - i] = hex_digits[value & 0xF];
        value >>= 4;
    }
    buffer[18] = '\0';

    write(buffer);
}

void Serial::handle_interrupt() 
{
    char c = port_byte_in(COM1_PORT);
    int next = (head_ + 1) % BUFFER_SIZE;
    if (next != tail_) {
        buffer[head_] = c;
        head_ = next;
    }
}

bool Serial::has_char()
{
    return head_ != tail_;
}

char Serial::read()
{
    if (head_ == tail_) return 0;
    char c = buffer[tail_];
    tail_ = (tail_ + 1) % BUFFER_SIZE;
    return c;
}