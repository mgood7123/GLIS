#include <unistd.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>

void libsu_LOG_ERROR(const char* format, ... ) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    va_end(args);
}

int main() {
    libsu_LOG_ERROR("(ppid: %ld, pid: %ld) before daemon", getppid(), getpid());
    if (fork() != 0) {
        pause();
    } else {
        daemon(1, 1);
        libsu_LOG_ERROR("(ppid: %ld, pid: %ld) after daemon", getppid(), getpid());
        while (true) {
            sleep(1);
        }
    }

}