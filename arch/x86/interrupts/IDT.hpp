#pragma once
#include <stddef.h>
#include <stdint.h>

class IDT 
{
private:
    IDT();
    ~IDT() = default;
    IDT(const IDT&) = delete;
    IDT& operator=(const IDT&) = delete;

public:
    static IDT& instance();

private:
    void install();
    void set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);

private:
    struct Entry {
        uint16_t base_low;
        uint16_t sel;
        uint8_t  always0;
        uint8_t  flags;
        uint16_t base_high;
    } __attribute__((packed));

    struct Ptr {
        uint16_t limit;
        uint32_t base;
    } __attribute__((packed));

    Entry idt_entries_[256];
    Ptr idt_ptr_;
};