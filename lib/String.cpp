#include "String.hpp"


int strcmp(const char* s1, const char* s2)
{
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return (unsigned char)(*s1) - (unsigned char)(*s2);
}

String::String(const char* data)
    : data_(data) {
}

const char* String::c_str() const
{
    return data_;
}