ENTRY(start)

SECTIONS
{
    . = 1M;

    .multiboot2 :
    {
        *(.multiboot2)
    }

    .text :
    {
        *(.text*)
    }

    .rodata :
    {
        *(.rodata*)
    }

    .data :
    {
        *(.data*)
    }

    .bss :
    {
        *(.bss*)
        *(COMMON)
    }

    kernel_end  = .;
}