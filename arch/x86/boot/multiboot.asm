section .multiboot2
align 8
header_start:
    dd 0xE85250D6              ; magic (Multiboot2)
    dd 0                       ; architecture (0 = i386)
    dd header_end - header_start ; header length
    dd -(0xE85250D6 + 0 + (header_end - header_start)) ; checksum

    dw 0    ; type = 0
    dw 0    ; flags = 0
    dd 8    ; size = 8
header_end:

section .text
extern kmain
global start

start:
    mov [magic], eax
    mov [mb_ptr], ebx

    push dword [mb_ptr]
    push dword [magic]
    call kmain

.hang:
    cli
    hlt
    jmp .hang

section .data
magic:   dd 0
mb_ptr:  dd 0