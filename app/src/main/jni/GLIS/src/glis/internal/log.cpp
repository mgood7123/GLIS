#include <glis/internal/log.hpp>

#ifndef __ANDROID__
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

int LOG_INFO(const char* format, ... ) {
    va_list args;
    va_start(args, format);
    // set color to green
    fprintf(stdout, "\033[0;32m");
    int len = vfprintf(stdout, format, args);
    // clear color
    fprintf(stdout, "\033[0m");
    len += fprintf(stdout, "\n");
    va_end(args);
    return len;
}

int LOG_ERROR(const char* format, ... ) {
    va_list args;
    va_start(args, format);
    // set color to red
    fprintf(stderr, "\033[0;31m");
    int len = vfprintf(stderr, format, args);
    // clear color
    fprintf(stderr, "\033[0m");
    len += fprintf(stderr, "\n");
    va_end(args);
    return len;
}

void LOG_ALWAYS_FATAL(const char* format, ... ) {
    va_list args;
    va_start(args, format);
    // set color to red
    fprintf(stderr, "\033[0;31m");
    vfprintf(stderr, format, args);
    // clear color
    fprintf(stderr, "\033[0m");
    fprintf(stderr, "\n");
    va_end(args);
    abort();
}
#endif