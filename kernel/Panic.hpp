#pragma once
#include <stdint.h>

namespace Kernel {
    [[noreturn]] void panic(const char* msg);
}