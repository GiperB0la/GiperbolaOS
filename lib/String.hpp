#pragma once
#include <stddef.h>
#include <stdint.h>

int strcmp(const char* s1, const char* s2);
size_t strlen(const char* str);

class String
{
public:
    String(const char* data);

public:
    const char* c_str() const;

private:
    const char* data_;
};