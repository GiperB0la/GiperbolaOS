global irq0_stub
global irq1_stub
; irq2_stub
global irq3_stub
global irq4_stub

extern irq0_handler
extern irq1_handler
; irq2_handler
extern irq3_handler
extern irq4_handler

irq0_stub:
    pusha
    call irq0_handler
    popa
    iretd

irq1_stub:
    pusha
    call irq1_handler
    popa
    iretd

; irq2_stub

irq3_stub:
    pusha
    call irq3_handler
    popa
    iretd

irq4_stub:
    pusha
    call irq4_handler
    popa
    iretd