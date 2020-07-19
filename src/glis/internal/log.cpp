#include <glis/internal/log.hpp>
#include <glis/internal/common.hpp>
#include <stdarg.h>

#ifdef __ANDROID__
#include <android/log.h>
#endif

#include <mutex>

std::mutex lock;

int LOG_INFO(const char* format, ... ) {
    assert(format != nullptr);
    lock.lock();
#ifdef __ANDROID__
    va_list args2;
    va_start(args2, format);
    __android_log_vprint(ANDROID_LOG_INFO, "GLIS", format, args2);
    va_end(args2);
#endif
    // set color to green
    fprintf(stdout, "\033[0;32m");
    va_list args;
    va_start(args, format);
    int len = vfprintf(stdout, format, args);
    va_end(args);
    // clear color
    fprintf(stdout, "\033[0m");
    len += fprintf(stdout, "\n");
    fflush(stdout);
    lock.unlock();
    return len;
}

int LOG_ERROR(const char* format, ... ) {
    assert(format != nullptr);
    lock.lock();
#ifdef __ANDROID__
    va_list args2;
    va_start(args2, format);
    __android_log_vprint(ANDROID_LOG_ERROR, "GLIS", format, args2);
    va_end(args2);
#endif
    // set color to red
    fprintf(stderr, "\033[0;31m");
    va_list args;
    va_start(args, format);
    int len = vfprintf(stderr, format, args);
    va_end(args);
    // clear color
    fprintf(stderr, "\033[0m");
    len += fprintf(stderr, "\n");
    fflush(stderr);
    lock.unlock();
    return len;
}

void LOG_ALWAYS_FATAL(const char* format, ... ) {
    assert(format != nullptr);
    lock.lock();
#ifdef __ANDROID__
    va_list args2;
    va_start(args2, format);
    __android_log_vprint(ANDROID_LOG_ERROR, "GLIS", format, args2);
    va_end(args2);
#endif
    // set color to red
    fprintf(stderr, "\033[0;31m");
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    // clear color
    fprintf(stderr, "\033[0m");
    fprintf(stderr, "\n");
    fflush(stderr);
    lock.unlock();
    abort();
}

#ifdef __ANDROID__
#include <Corrade/Utility/AndroidLogStreamBuffer.h>

Corrade::Utility::AndroidLogStreamBuffer bufferDebug {Corrade::Utility::AndroidLogStreamBuffer::LogPriority::Debug, "GLIS-MAGNUM"};
std::ostream streamDebug {&bufferDebug};

Corrade::Utility::AndroidLogStreamBuffer bufferWarning {Corrade::Utility::AndroidLogStreamBuffer::LogPriority::Warning, "GLIS-MAGNUM"};
std::ostream streamWarning {&bufferWarning};

Corrade::Utility::AndroidLogStreamBuffer bufferError {Corrade::Utility::AndroidLogStreamBuffer::LogPriority::Error, "GLIS-MAGNUM"};
std::ostream streamError {&bufferError};

Corrade::Utility::AndroidLogStreamBuffer bufferFatal {Corrade::Utility::AndroidLogStreamBuffer::LogPriority::Fatal, "GLIS-MAGNUM"};
std::ostream streamFatal {&bufferFatal};
#endif