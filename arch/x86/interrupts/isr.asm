global isr0_stub
extern isr0_handler

isr0_stub:
    pusha                ; сохраняем все регистры
    call isr0_handler    ; зовём нашу C++ функцию
    popa
    add esp, 4           ; поправим стек, если есть код ошибки (для исключений 0x08,0x0A-0x0E,0x11)
    iretd