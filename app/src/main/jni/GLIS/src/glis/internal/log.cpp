#include <glis/internal/log.hpp>

#ifndef __ANDROID__
void LOG_INFO(const char* format, ... ) {
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    fprintf(stdout, "\n");
    va_end(args);
}

void LOG_ERROR(const char* format, ... ) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    va_end(args);
}
#endif