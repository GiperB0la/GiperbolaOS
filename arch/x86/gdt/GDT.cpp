#include "GDT.hpp"

extern "C" void gdt_flush(uint32_t);

GDT::GDT()
{
    install();
}

GDT& GDT::instance()
{
    static GDT gdt;
    return gdt;
}

void GDT::install() 
{
    gdt_ptr.limit = (sizeof(Entry) * 3) - 1;
    gdt_ptr.base  = (uint32_t)&gdt_entries;

    set_gate(0, 0, 0, 0, 0);                       // Null
    set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);        // Code segment
    set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);        // Data segment

    gdt_flush((uint32_t)&gdt_ptr);
}

void GDT::set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) 
{
    gdt_entries[num].base_low    = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high   = (base >> 24) & 0xFF;

    gdt_entries[num].limit_low   = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;

    gdt_entries[num].granularity |= (gran & 0xF0);
    gdt_entries[num].access       = access;
}