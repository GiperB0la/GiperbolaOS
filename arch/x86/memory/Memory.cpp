#include "Memory.hpp"


Memory::Memory(uint32_t mb_addr)
    : mb_addr(mb_addr)
{
    init();
}

Memory& Memory::instance(uint32_t mb_addr)
{
    static Memory memory(mb_addr);
    return memory;
}

void Memory::init()
{
    total_pages = 0;
    free_pages = 0;

    auto* tag = reinterpret_cast<multiboot_tag*>(mb_addr + 8);
    while (tag->type != 0) {
        if (tag->type == 6) {
            auto* mmap_tag = reinterpret_cast<multiboot_tag_mmap*>(tag);
            auto* entry = reinterpret_cast<multiboot_mmap_entry*>(mmap_tag + 1);

            while ((uint8_t*)entry < (uint8_t*)mmap_tag + mmap_tag->size) {
                if (entry->type == 1) { // usable
                    total_pages += entry->len / PAGE_SIZE;
                }
                entry = reinterpret_cast<multiboot_mmap_entry*>(
                    (uint8_t*)entry + mmap_tag->entry_size
                );
            }
        }

        tag = reinterpret_cast<multiboot_tag*>(
            (uint8_t*)tag + ((tag->size + 7) & ~7)
        );
    }

    free_pages = total_pages;

    extern uint32_t end;
    bitmap = reinterpret_cast<uint32_t*>(&end);

    for (size_t i = 0; i < total_pages / 32; i++) {
        bitmap[i] = 0xFFFFFFFF;
    }

    tag = reinterpret_cast<multiboot_tag*>(mb_addr + 8);
    while (tag->type != 0) {
        if (tag->type == 6) {
            auto* mmap_tag = reinterpret_cast<multiboot_tag_mmap*>(tag);
            auto* entry = reinterpret_cast<multiboot_mmap_entry*>(mmap_tag + 1);

            while ((uint8_t*)entry < (uint8_t*)mmap_tag + mmap_tag->size) {
                if (entry->type == 1) {
                    for (uint64_t a = entry->addr; a < entry->addr + entry->len; a += PAGE_SIZE) {
                        size_t idx = a / PAGE_SIZE;
                        clearBit(idx);
                    }
                }
                entry = reinterpret_cast<multiboot_mmap_entry*>(
                    (uint8_t*)entry + mmap_tag->entry_size
                );
            }
        }
        tag = reinterpret_cast<multiboot_tag*>(
            (uint8_t*)tag + ((tag->size + 7) & ~7)
        );
    }

    for (uintptr_t a = 0; a < 0x100000; a += PAGE_SIZE) {
        setBit(a / PAGE_SIZE);
    }
}

void* Memory::allocPage() 
{
    for (size_t i = 0; i < total_pages; i++) {
        if (!testBit(i)) {
            setBit(i);
            free_pages--;
            return reinterpret_cast<void*>(i * PAGE_SIZE);
        }
    }
    return nullptr;
}

void Memory::freePage(void* addr) 
{
    size_t idx = reinterpret_cast<uintptr_t>(addr) / PAGE_SIZE;
    if (testBit(idx)) {
        clearBit(idx);
        free_pages++;
    }
}

size_t Memory::getFreeMemory() 
{
    return free_pages * PAGE_SIZE;
}

void Memory::setBit(size_t idx) 
{
    bitmap[idx / 32] |= (1 << (idx % 32));
}

void Memory::clearBit(size_t idx) 
{
    bitmap[idx / 32] &= ~(1 << (idx % 32));
}

bool Memory::testBit(size_t idx) 
{
    return bitmap[idx / 32] & (1 << (idx % 32));
}

uint32_t Memory::getMbAddr() const
{
    return mb_addr;
}