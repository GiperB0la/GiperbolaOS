#pragma once
#include <stddef.h>
#include <stdint.h>

class GDT
{
private:
    GDT();

public:
    static GDT& instance();

private:
    void install();
    void set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

private:
    struct Entry {
        uint16_t limit_low;
        uint16_t base_low;
        uint8_t  base_middle;
        uint8_t  access;
        uint8_t  granularity;
        uint8_t  base_high;
    } __attribute__((packed));

    struct Ptr {
        uint16_t limit;
        uint32_t base;
    } __attribute__((packed));

    Entry gdt_entries[3];
    Ptr gdt_ptr;
};