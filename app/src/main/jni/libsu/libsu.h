#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#ifndef __ANDROID__
    void libsu_LOG_INFO(const char* format, ... );
    void libsu_LOG_ERROR(const char* format, ... );
#else
    #ifndef ANDROID_LOG_INFO
        #include <android/log.h>
    #else
        #ifndef ANDROID_LOG_ERROR
            #include <android/log.h>
        #endif
    #endif
    #define libsu_LOG_TAG "libsu"
    #ifndef libsu_LOG_INFO
        #define libsu_LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO, libsu_LOG_TAG, __VA_ARGS__)
    #endif
    #ifndef libsu_LOG_ERROR
        #define libsu_LOG_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, libsu_LOG_TAG, __VA_ARGS__)
    #endif
#endif

struct libsu_processimage {
    pid_t pid;
    int stdin_fd;
    int stdout_fd;
    char * string_stdout;
    bool stdout_should_be_freed;
    int stderr_fd;
    char * string_stderr;
    bool stderr_should_be_freed;
    bool exited_normally;
    int return_code;
    bool exited_from_signal;
    int signal;
};

typedef struct libsu_processimage libsu_processimage;

bool libsu_has_root_access();
bool libsu_sudo(libsu_processimage * instance, const char * command);
// in magisk su, an option exists, called --mount-master,
// which executes commands in the global mount namespace
bool libsu_sudo(libsu_processimage * instance, bool mount_master, const char * command);
void libsu_print_info(libsu_processimage * instance, const char * command);
void libsu_print_info(libsu_processimage * instance, bool mount_master, const char * command);
void libsu_print_info(libsu_processimage * instance, bool libsu_sudo_return_code);
void libsu_cleanup(libsu_processimage * instance);
// exits THIS process and continue execution in child process
void libsu_daemon();