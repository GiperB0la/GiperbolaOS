#include "String.hpp"


int strcmp(const char* s1, const char* s2)
{
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return (unsigned char)(*s1) - (unsigned char)(*s2);
}

size_t strlen(const char* str)
{
    size_t len = 0;
    while (str[len] != '\0') {
        ++len;
    }
    return len;
}

String::String(const char* data)
    : data_(data) {
}

const char* String::c_str() const
{
    return data_;
}