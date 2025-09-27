#include "../../../drivers/VGA.hpp"
#include "../../../kernel/Panic.hpp"

extern "C" void isr0_handler() {
    Kernel::panic("Divide by zero");
}