#include "Pic.hpp"
#include "../io/LowLevel_io.hpp"

#define PIC1            0x20
#define PIC2            0xA0
#define PIC1_COMMAND    PIC1
#define PIC1_DATA       (PIC1+1)
#define PIC2_COMMAND    PIC2
#define PIC2_DATA       (PIC2+1)

#define ICW1_INIT       0x10
#define ICW1_ICW4       0x01
#define ICW4_8086       0x01
#define PIC_EOI         0x20

void pic_remap(int offset1, int offset2)
{
    uint8_t a1 = port_byte_in(PIC1_DATA); // сохранить маски
    uint8_t a2 = port_byte_in(PIC2_DATA);

    port_byte_out(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    port_byte_out(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);

    port_byte_out(PIC1_DATA, offset1); // новая база IRQ0..7
    port_byte_out(PIC2_DATA, offset2); // новая база IRQ8..15

    port_byte_out(PIC1_DATA, 4); // указываем slave на IRQ2
    port_byte_out(PIC2_DATA, 2);

    port_byte_out(PIC1_DATA, ICW4_8086);
    port_byte_out(PIC2_DATA, ICW4_8086);

    port_byte_out(PIC1_DATA, a1); // восстановить маски
    port_byte_out(PIC2_DATA, a2);
}

void pic_send_eoi(uint8_t irq) 
{
    if (irq >= 8) {
        port_byte_out(PIC2_COMMAND, PIC_EOI);
    }
    port_byte_out(PIC1_COMMAND, PIC_EOI);
}