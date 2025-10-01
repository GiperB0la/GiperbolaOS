#include "Memory.hpp"
#include "../../../drivers/Serial.hpp"
#include "../../../drivers/VGA.hpp"


Memory::Memory(uint32_t* mb_addr)
    : mb_addr_(mb_addr)
{
    init();
}

Memory& Memory::instance(uint32_t* mb_addr)
{
    static Memory memory(mb_addr);
    return memory;
}

extern char kernel_end;

void Memory::init()
{
    total_pages_ = 0;

    multiboot_tag* tag = reinterpret_cast<multiboot_tag*>((uint8_t*)mb_addr_ + 8);

    while (tag->type_ != MULTIBOOT_TAG_TYPE_END) {
        if (tag->type_ == MULTIBOOT_TAG_TYPE_MMAP) {
            auto* mmap_tag = reinterpret_cast<multiboot_tag_mmap*>(tag);
            auto* entry = reinterpret_cast<multiboot_mmap_entry*>(mmap_tag + 1);

            while ((uint8_t*)entry < (uint8_t*)mmap_tag + mmap_tag->size_) {
                if (entry->type_ == MULTIBOOT_MEMORY_AVAILABLE) {
                    total_pages_ += entry->len_ / PAGE_SIZE;
                }
                entry = reinterpret_cast<multiboot_mmap_entry*>((uint8_t*)entry + mmap_tag->entry_size_);
            }
        }

        tag = reinterpret_cast<multiboot_tag*>((uint8_t*)tag + ((tag->size_ + 7) & ~7));
    }

    size_t bmp_size_words = (total_pages_ + 31) / 32;
    size_t bmp_size_bytes = bmp_size_words * sizeof(uint32_t);

    uintptr_t kernel_start = 0x00100000;
    uintptr_t kernel_end_addr = (uintptr_t)&kernel_end;

    uint32_t total_size = *(uint32_t*)mb_addr_;
    uintptr_t mbi_start = (uintptr_t)mb_addr_;
    uintptr_t mbi_end   = mbi_start + total_size;

    uintptr_t safe_start = (kernel_end_addr > mbi_end) ? kernel_end_addr : mbi_end;
    uintptr_t bitmap_addr = (safe_start + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);

    bitmap_ = reinterpret_cast<uint32_t*>(bitmap_addr);

    uintptr_t bitmap_start = bitmap_addr;
    uintptr_t bitmap_end   = bitmap_start + bmp_size_bytes;

    for (size_t i = 0; i < bmp_size_words; i++) {
        bitmap_[i] = 0xFFFFFFFF;
    }

    free_pages_ = 0;
    tag = reinterpret_cast<multiboot_tag*>((uint8_t*)mb_addr_ + 8);

    while (tag->type_ != MULTIBOOT_TAG_TYPE_END) {
        if (tag->type_ == MULTIBOOT_TAG_TYPE_MMAP) {
            auto* mmap_tag = reinterpret_cast<multiboot_tag_mmap*>(tag);
            auto* entry = reinterpret_cast<multiboot_mmap_entry*>(mmap_tag + 1);

            while ((uint8_t*)entry < (uint8_t*)mmap_tag + mmap_tag->size_) {
                if (entry->type_ == MULTIBOOT_MEMORY_AVAILABLE) {
                    uint64_t start = entry->addr_;
                    uint64_t end   = entry->addr_ + entry->len_;

                    if (start < 0x100000) {
                        start = 0x100000;
                        if (end <= start) {
                            entry = reinterpret_cast<multiboot_mmap_entry*>((uint8_t*)entry + mmap_tag->entry_size_);
                            continue;
                        }
                    }

                    for (uint64_t addr = start; addr < end; addr += PAGE_SIZE) {
                        uint64_t page_start = addr;
                        uint64_t page_end   = addr + PAGE_SIZE;

                        if ((page_start < kernel_end_addr && page_end > kernel_start) ||
                            (page_start < bitmap_end     && page_end > bitmap_start) ||
                            (page_start < mbi_end        && page_end > mbi_start)) {
                            continue;
                        }

                        size_t page_idx = addr / PAGE_SIZE;
                        if (page_idx >= total_pages_) {
                            continue;
                        }

                        clear_bit(page_idx);
                        free_pages_++;
                    }
                }
                entry = reinterpret_cast<multiboot_mmap_entry*>((uint8_t*)entry + mmap_tag->entry_size_);
            }
        }
        tag = reinterpret_cast<multiboot_tag*>((uint8_t*)tag + ((tag->size_ + 7) & ~7));
    }
}

void Memory::set_bit(size_t idx)
{
    bitmap_[idx / 32] |= (1 << (idx % 32));
}

void Memory::clear_bit(size_t idx)
{
    bitmap_[idx / 32] &= ~(1 << (idx % 32));
}

bool Memory::test_bit(size_t idx)
{
    return bitmap_[idx / 32] & (1 << (idx % 32));
}

void Memory::memory_info()
{
    VGA::instance().print("\n\n=========== MEMORY INFO ============\n");

    VGA::instance().print("Total available memory: ");
    VGA::instance().print((unsigned int)(total_pages_ * PAGE_SIZE / 1024));
    VGA::instance().print(" KB\n");

    VGA::instance().print("Total pages (4KB): ");
    VGA::instance().print((unsigned int)total_pages_);
    VGA::instance().print("\n");

    extern char kernel_end;
    uintptr_t kernel_start = 0x00100000;
    uintptr_t kernel_end_addr = (uintptr_t)&kernel_end;

    VGA::instance().print("Kernel: ");
    VGA::instance().print_hex((unsigned int)kernel_start);
    VGA::instance().print(" - ");
    VGA::instance().print_hex((unsigned int)kernel_end_addr);
    VGA::instance().print("\n");

    uint32_t total_size = *(uint32_t*)mb_addr_;
    uintptr_t mbi_start = (uintptr_t)mb_addr_;
    uintptr_t mbi_end   = mbi_start + total_size;

    VGA::instance().print("MBI:    ");
    VGA::instance().print_hex((unsigned int)mbi_start);
    VGA::instance().print(" - ");
    VGA::instance().print_hex((unsigned int)mbi_end);
    VGA::instance().print("\n");

    size_t bmp_size_words = (total_pages_ + 31) / 32;
    size_t bmp_size_bytes = bmp_size_words * sizeof(uint32_t);
    uintptr_t bitmap_start = (uintptr_t)bitmap_;
    uintptr_t bitmap_end   = bitmap_start + bmp_size_bytes;

    VGA::instance().print("Bitmap: ");
    VGA::instance().print_hex((unsigned int)bitmap_start);
    VGA::instance().print(" - ");
    VGA::instance().print_hex((unsigned int)bitmap_end);
    VGA::instance().print("\n");

    VGA::instance().print("------------------------------------\n");
    VGA::instance().print("Memory map:\n");

    multiboot_tag* tag = reinterpret_cast<multiboot_tag*>((uint8_t*)mb_addr_ + 8);
    while (tag->type_ != MULTIBOOT_TAG_TYPE_END) {
        if (tag->type_ == MULTIBOOT_TAG_TYPE_MMAP) {
            auto* mmap_tag = reinterpret_cast<multiboot_tag_mmap*>(tag);
            auto* entry = reinterpret_cast<multiboot_mmap_entry*>(mmap_tag + 1);

            while ((uint8_t*)entry < (uint8_t*)mmap_tag + mmap_tag->size_) {
                uintptr_t start = entry->addr_;
                uintptr_t end   = entry->addr_ + entry->len_;

                VGA::instance().print("[");
                VGA::instance().print_hex((unsigned int)start);
                VGA::instance().print(" - ");
                VGA::instance().print_hex((unsigned int)end);
                VGA::instance().print("] ");

                if (entry->type_ == MULTIBOOT_MEMORY_AVAILABLE) {
                    VGA::instance().print("AVAILABLE\n");
                } 
                else {
                    VGA::instance().print("RESERVED\n");
                }

                entry = reinterpret_cast<multiboot_mmap_entry*>((uint8_t*)entry + mmap_tag->entry_size_);
            }
        }
        tag = reinterpret_cast<multiboot_tag*>((uint8_t*)tag + ((tag->size_ + 7) & ~7));
    }

    VGA::instance().print("====================================\n");
}

int Memory::find_first_free() 
{
    size_t bmp_size_words = (total_pages_ + 31) / 32;
    for (size_t i = 0; i < bmp_size_words; i++) {
        if (bitmap_[i] != 0xFFFFFFFF) {
            for (int b = 0; b < 32; b++) {
                if (!(bitmap_[i] & (1 << b))) {
                    return i * 32 + b;
                }
            }
        }
    }
    return -1;
}

int Memory::find_first_free_pages(size_t count) 
{
    size_t free_run = 0;
    size_t start_idx = 0;

    for (size_t i = 0; i < total_pages_; i++) {
        if (!test_bit(i)) {
            if (free_run == 0) start_idx = i;
            free_run++;
            if (free_run == count) return start_idx;
        } else {
            free_run = 0;
        }
    }
    return -1;
}

void* Memory::alloc_page()
{
    int idx = find_first_free();
    if (idx == -1) return nullptr;
    set_bit(idx);
    free_pages_--;
    return (void*)(idx * PAGE_SIZE);
}

void* Memory::alloc_pages(size_t count) 
{
    int idx = find_first_free_pages(count);
    if (idx == -1) return nullptr;

    for (size_t i = 0; i < count; i++) {
        set_bit(idx + i);
        free_pages_--;
    }
    return (void*)(idx * PAGE_SIZE);
}

void Memory::free_page(void* addr) 
{
    size_t idx = (uintptr_t)addr / PAGE_SIZE;
    if (test_bit(idx)) {
        clear_bit(idx);
        free_pages_++;
    }
}

void Memory::free_pages(void* addr, size_t count)
{
    size_t idx = (uintptr_t)addr / PAGE_SIZE;
    for (size_t i = 0; i < count; i++) {
        if (test_bit(idx + i)) {
            clear_bit(idx + i);
            free_pages_++;
        }
    }
}