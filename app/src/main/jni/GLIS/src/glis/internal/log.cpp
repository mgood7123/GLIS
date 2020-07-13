#include <glis/internal/log.hpp>

#ifndef __ANDROID__
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

int LOG_INFO(const char* format, ... ) {
    va_list args;
    va_start(args, format);
    int len = vfprintf(stdout, format, args);
    len += fprintf(stdout, "\n");
    va_end(args);
    return len;
}

int LOG_ERROR(const char* format, ... ) {
    va_list args;
    va_start(args, format);
    int len = vfprintf(stderr, format, args);
    len += fprintf(stderr, "\n");
    va_end(args);
    return len;
}

void LOG_ALWAYS_FATAL(const char* format, ... ) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    va_end(args);
    abort();
}
#endif