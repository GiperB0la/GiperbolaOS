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
    gdt_ptr_.limit_ = (sizeof(Entry) * 3) - 1;
    gdt_ptr_.base_  = (uint32_t)&gdt_entries_;

    set_gate(0, 0, 0, 0, 0);                       // Null
    set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);        // Code segment
    set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);        // Data segment

    gdt_flush((uint32_t)&gdt_ptr_);
}

void GDT::set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) 
{
    gdt_entries_[num].base_low_    = (base & 0xFFFF);
    gdt_entries_[num].base_middle_ = (base >> 16) & 0xFF;
    gdt_entries_[num].base_high_   = (base >> 24) & 0xFF;

    gdt_entries_[num].limit_low_   = (limit & 0xFFFF);
    gdt_entries_[num].granularity_ = (limit >> 16) & 0x0F;

    gdt_entries_[num].granularity_ |= (gran & 0xF0);
    gdt_entries_[num].access_       = access;
}