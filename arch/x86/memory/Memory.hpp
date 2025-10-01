#pragma once
#include <stdint.h>
#include <stddef.h>
#include "multiboot.hpp"

#define PAGE_SIZE 4096
#define BLOCK_SIZE 4096

class Memory
{
private:
    Memory(uint32_t* mb_addr);
    ~Memory() = default;
    Memory(const Memory&) = delete;
    Memory& operator=(const Memory&) = delete;

public:
    static Memory& instance(uint32_t* mb_addr = nullptr);

public:
    void memory_info();
    void* alloc_page();
    void* alloc_pages(size_t count);
    void free_page(void* ptr);
    void free_pages(void* addr, size_t count);
    size_t get_total_pages() const { return total_pages_; }
    size_t get_free_pages() const { return free_pages_; }

private:
    void init();
    void set_bit(size_t idx);
    void clear_bit(size_t idx);
    bool test_bit(size_t idx);
    int find_first_free();
    int find_first_free_pages(size_t count);

private:
    uint32_t* mb_addr_;
    uint32_t* bitmap_;
    size_t bmp_size_;
    size_t total_pages_;
    size_t free_pages_;
};