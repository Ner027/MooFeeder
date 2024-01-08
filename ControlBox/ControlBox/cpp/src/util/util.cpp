#include <cstdarg>
#include <cstdio>
#include "util/util.h"

void print(const char* func, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    printf("[%s] ", func);
    vprintf(format, args);
    va_end(args);
}