global isr0_stub
extern isr0_handler

isr0_stub:
    pusha
    call isr0_handler
    popa
    add esp, 4
    iretd