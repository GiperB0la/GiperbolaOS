#include "Pic.hpp"
#include "../io/LowLevel_io.hpp"


void pic_remap(int offset1, int offset2)
{
    uint8_t a1 = port_byte_in(PIC1_DATA);
    uint8_t a2 = port_byte_in(PIC2_DATA);

    port_byte_out(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    port_byte_out(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);

    port_byte_out(PIC1_DATA, offset1);
    port_byte_out(PIC2_DATA, offset2);

    port_byte_out(PIC1_DATA, 4);
    port_byte_out(PIC2_DATA, 2);

    port_byte_out(PIC1_DATA, ICW4_8086);
    port_byte_out(PIC2_DATA, ICW4_8086);

    port_byte_out(PIC1_DATA, a1);
    port_byte_out(PIC2_DATA, a2);
}

void pic_send_eoi(uint8_t irq) 
{
    if (irq >= 8) {
        port_byte_out(PIC2_COMMAND, PIC_EOI);
    }
    port_byte_out(PIC1_COMMAND, PIC_EOI);
}