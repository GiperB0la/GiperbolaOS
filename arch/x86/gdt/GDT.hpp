#pragma once
#include <stddef.h>
#include <stdint.h>

class GDT
{
private:
    GDT();
    ~GDT() = default;
    GDT(const GDT&) = delete;
    GDT& operator=(const GDT&) = delete;

public:
    static GDT& instance();

private:
    void install();
    void set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

private:
    struct Entry {
        uint16_t limit_low_;
        uint16_t base_low_;
        uint8_t  base_middle_;
        uint8_t  access_;
        uint8_t  granularity_;
        uint8_t  base_high_;
    } __attribute__((packed));

    struct Ptr {
        uint16_t limit_;
        uint32_t base_;
    } __attribute__((packed));

    Entry gdt_entries_[3];
    Ptr gdt_ptr_;
};