#include "IDT.hpp"
#include "Pic.hpp"

extern "C" void idt_load(uint32_t);

extern "C" void isr0_stub();

extern "C" void irq0_stub();
extern "C" void irq1_stub();
// irq2_stub
extern "C" void irq3_stub();
extern "C" void irq4_stub();

IDT::IDT()
{
    install();
}

IDT& IDT::instance()
{
    static IDT idt;
    return idt;
}

void IDT::install()
{
    pic_remap(0x20, 0x28);

    idt_ptr_.limit = sizeof(Entry) * 256 - 1;
    idt_ptr_.base  = (uint32_t)&idt_entries_;

    for (int i = 0; i < 256; i++) {
        set_gate(i, 0, 0, 0);
    }

    // isrs
    set_gate(0x00, (uint32_t)isr0_stub, 0x08, 0x8E); // ISR0 (Division by zero)
    
    // irqs
    set_gate(0x20, (uint32_t)irq0_stub, 0x08, 0x8E); // IRQ0 (Timer)
    set_gate(0x21, (uint32_t)irq1_stub, 0x08, 0x8E); // IRQ1 (Keyboard)
    // irq2
    set_gate(0x23, (uint32_t)irq3_stub, 0x08, 0x8E); // IRQ3 (COM2)
    set_gate(0x24, (uint32_t)irq4_stub, 0x08, 0x8E); // IRQ4 (COM1)

    idt_load((uint32_t)&idt_ptr_);
}

void IDT::set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
    idt_entries_[num].base_low  = base & 0xFFFF;
    idt_entries_[num].sel       = sel;
    idt_entries_[num].always0   = 0;
    idt_entries_[num].flags     = flags;
    idt_entries_[num].base_high = (base >> 16) & 0xFFFF;
}