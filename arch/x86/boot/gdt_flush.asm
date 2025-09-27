global gdt_flush
gdt_flush:
    mov eax, [esp+4]    ; адрес структуры GDT::Ptr
    lgdt [eax]

    mov ax, 0x10        ; data segment selector = 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    jmp 0x08:.flush     ; far jump для обновления CS
.flush:
    ret