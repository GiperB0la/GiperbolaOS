#pragma once
#include <stdint.h>
#include <stddef.h>
#include "multiboot.hpp"

#define PAGE_SIZE 4096

class Memory
{
private:
    Memory(uint32_t mb_addr);

public:
    static Memory& instance(uint32_t mb_addr = 0);

public:
    void* allocPage();
    void freePage(void* addr);
    size_t getFreeMemory();
    uint32_t getMbAddr() const;

private:
    void init();
    void setBit(size_t idx);
    void clearBit(size_t idx);
    bool testBit(size_t idx);

private:
    uint32_t* bitmap = nullptr;
    size_t total_pages = 0;
    size_t free_pages = 0;
    uint32_t mb_addr;
};