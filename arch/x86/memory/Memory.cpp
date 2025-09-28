#include "Memory.hpp"
#include "../../../drivers/Serial.hpp"
#include "../../../drivers/VGA.hpp"


Memory::Memory(uint32_t* mb_addr)
    : mb_addr(mb_addr)
{
    init();
}

Memory& Memory::instance(uint32_t* mb_addr)
{
    static Memory memory(mb_addr);
    return memory;
}

void Memory::init()
{

}

void Memory::memory_info()
{
    Serial::instance().write("\n================= Memory map =================\n");
    VGA::instance().print("\n================= Memory map =================\n");

    auto* tag = reinterpret_cast<multiboot_tag*>((uint8_t*)mb_addr + 8);

    uint64_t total_available = 0;

    while (tag->type != MULTIBOOT_TAG_TYPE_END) {
        if (tag->type == MULTIBOOT_TAG_TYPE_MMAP) {
            auto* mmap_tag = reinterpret_cast<multiboot_tag_mmap*>(tag);
            auto* entry = mmap_tag->entries;

            while ((uint8_t*)entry < (uint8_t*)mmap_tag + mmap_tag->size) {
                Serial::instance().write("Base: ");
                Serial::instance().write_hex((unsigned int)(entry->addr));
                Serial::instance().write("  Length: ");
                Serial::instance().write_hex((unsigned int)(entry->len));
                Serial::instance().write("  Type: ");
                Serial::instance().write((unsigned int)entry->type);
                Serial::instance().write("\n");

                VGA::instance().print("Base: ");
                VGA::instance().print_hex((unsigned int)(entry->addr));
                VGA::instance().print("  Length: ");
                VGA::instance().print_hex((unsigned int)(entry->len));
                VGA::instance().print("  Type: ");
                VGA::instance().print((unsigned int)entry->type);
                VGA::instance().print("\n");

                if (entry->type == MULTIBOOT_MEMORY_AVAILABLE) {
                    total_available += entry->len;
                }

                entry = reinterpret_cast<multiboot_tag_mmap::multiboot_mmap_entry*>(
                    (uint8_t*)entry + mmap_tag->entry_size
                );
            }
        }

        tag = reinterpret_cast<multiboot_tag*>(
            (uint8_t*)tag + ((tag->size + 7) & ~7)
        );
    }

    Serial::instance().write("==============================================\n\n");
    VGA::instance().print("==============================================\n\n");

    uint64_t total_kb = total_available / 1024;
    uint64_t total_pages = total_available / 4096;

    Serial::instance().write("Total available memory: ");
    Serial::instance().write((unsigned int)total_kb);
    Serial::instance().write(" KB\n");

    Serial::instance().write("Total pages (4KB): ");
    Serial::instance().write((unsigned int)total_pages);
    Serial::instance().write("\n");

    VGA::instance().print("Total available memory: ");
    VGA::instance().print((unsigned int)total_kb);
    VGA::instance().print(" KB\n");

    VGA::instance().print("Total pages (4KB): ");
    VGA::instance().print((unsigned int)total_pages);
    VGA::instance().print("\n");
}

void Memory::set_bit(size_t idx)
{
    bitmap[idx / 32] |= (1 << (idx % 32));
}

void Memory::clear_bit(size_t idx)
{
    bitmap[idx / 32] &= ~(1 << (idx % 32));
}

bool Memory::test_bit(size_t idx)
{
    return bitmap[idx / 32] & (1 << (idx % 32));
}