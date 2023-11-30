#include "../inc/util.h"
#include <stdarg.h>
#include <stdio.h>

void print(const char* func, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    printf("[%s] ", func);
    vprintf(format, args);
    va_end(args);
}