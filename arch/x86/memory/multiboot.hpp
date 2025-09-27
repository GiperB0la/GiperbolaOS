#pragma once
#include <stdint.h>

#define MULTIBOOT2_BOOTLOADER_MAGIC 0x36d76289

struct multiboot_tag {
    uint32_t type;
    uint32_t size;
};

// --- Теги ---
// Имя загрузчика
struct multiboot_tag_string {
    uint32_t type;
    uint32_t size;
    char string[0];
};

// Командная строка
struct multiboot_tag_cmdline {
    uint32_t type;
    uint32_t size;
    char string[0];
};

// Карта памяти (E820)
struct multiboot_tag_mmap {
    uint32_t type;
    uint32_t size;
    uint32_t entry_size;
    uint32_t entry_version;
    // массив записей идёт сразу после
};

// Одна запись карты памяти
struct multiboot_mmap_entry {
    uint64_t addr; // физический адрес
    uint64_t len;  // размер области
    uint32_t type; // 1 = usable, другое = занято
    uint32_t zero; // reserved
};