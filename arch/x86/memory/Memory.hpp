#pragma once
#include <stdint.h>
#include <stddef.h>
#include "multiboot.hpp"

#define PAGE_SIZE 4096

class Memory
{
private:
    Memory(uint32_t* mb_addr);

public:
    static Memory& instance(uint32_t* mb_addr = nullptr);

public:
    void memory_info();

private:
    void init();
    void set_bit(size_t idx);
    void clear_bit(size_t idx);
    bool test_bit(size_t idx);

private:
    uint32_t* mb_addr;
    uint32_t* bitmap;
    size_t bmp_size;
};