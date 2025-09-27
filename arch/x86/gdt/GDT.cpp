#include "GDT.hpp"

extern "C" void gdt_flush(uint32_t); // ASM

GDT::GDT()
{
    install();
}

GDT& GDT::instance()
{
    static GDT gdt;
    gdt.install();
    return gdt;
}

void GDT::install() 
{
    gdt_ptr.limit = (sizeof(Entry) * 3) - 1;
    gdt_ptr.base  = (uint32_t)&gdt_entries;

    setGate(0, 0, 0, 0, 0);                       // Null
    setGate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);        // Code segment
    setGate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);        // Data segment

    gdt_flush((uint32_t)&gdt_ptr);
}

void GDT::setGate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) 
{
    gdt_entries[num].base_low    = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high   = (base >> 24) & 0xFF;

    gdt_entries[num].limit_low   = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;

    gdt_entries[num].granularity |= (gran & 0xF0);
    gdt_entries[num].access       = access;
}