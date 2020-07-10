#include "libsu.h"

#ifndef __ANDROID__
void libsu_LOG_INFO(const char* format, ... ) {
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    fprintf(stdout, "\n");
    va_end(args);
}

void libsu_LOG_ERROR(const char* format, ... ) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    va_end(args);
}
#endif

bool libsu_has_root_access() {
    libsu_processimage instance;
    bool r = libsu_sudo(&instance, "true");
    libsu_LOG_INFO("libsu returned: %s", r ? "true" : "false");
    libsu_LOG_INFO("libsu instance return code: %d", instance.return_code);
    libsu_LOG_INFO("libsu stdout: %s", instance.string_stdout);
    libsu_LOG_INFO("libsu stderr: %s", instance.string_stderr);
    int return_code = instance.return_code;
    libsu_cleanup(&instance);
    return return_code == 0;
}

void libsu_processimage_clear(libsu_processimage * instance) {
    instance->pid = 0;
    instance->stdin_fd = 0;
    instance->stdout_fd = 0;
    instance->string_stdout = NULL;
    instance->stdout_should_be_freed = false;
    instance->stderr_fd = 0;
    instance->string_stderr = NULL;
    instance->stderr_should_be_freed = false;
    instance->exited_normally = false;
    instance->return_code = 0;
    instance->exited_from_signal = false;
    instance->signal = 0;
}

int libsu_read_fd(int fd, char ** outString, bool * mustBeFreed) {
    if (outString != NULL && mustBeFreed != NULL) {
        size_t len = 4096 * sizeof(char);
#ifdef __cplusplus
        *outString = reinterpret_cast<char *>(malloc(len));
#else
        *outString = malloc(len);
#endif
        *mustBeFreed = true;
        memset(*outString, '\0', len);
        // steal code from socket read implementation
        int bytes = 0;
        for (;;) { // implement blocking
            ssize_t ret_ = read(fd, *outString, 4096);
            int err_ = errno;
            if (ret_ < 0) {
                if (err_ == EAGAIN || err_ == EWOULDBLOCK || err_ == EINTR) {
                    if (bytes == 0) continue;
                    else break;
                } else {
                    const char *e = strerror(err_);
                    libsu_LOG_INFO("libsu - read return an error: (errno: %2d) %s", err_, e);
                    return -1;
                }
            } else if (ret_ == 0) {
                libsu_LOG_INFO("libsu - read returned EOF (return code: 0)");
                return 0;
            } else {
                bytes = ret_;
            }
        }
        return 0;
    }
    return -1;
}

int libsu_read(libsu_processimage * instance) {
    libsu_read_fd(instance->stdout_fd, &instance->string_stdout, &instance->stdout_should_be_freed);
    libsu_read_fd(instance->stderr_fd, &instance->string_stderr, &instance->stderr_should_be_freed);
    return 0;
}

bool libsu_sudo(libsu_processimage * instance, const char * command) {
    assert(command != NULL);
    libsu_processimage_clear(instance);
    libsu_LOG_INFO("attempting to invoke command: %s", command);
    int stdin_fd[2], stdout_fd[2], stderr_fd[2];
    pid_t child;
    bool in = false;
    bool out = true;
    bool err = true;

    if(in)
        if(pipe(stdin_fd))
            return false;
    if(out)
        if(pipe(stdout_fd))
            return false;
    if(err)
        if(pipe(stderr_fd))
            return false;

    switch((child = fork())) {
        case -1:
            return false;

        case 0:
            if(in) {
                close(STDIN_FILENO);
                dup(stdin_fd[0]);
                close(stdin_fd[1]);
            }
            if(out) {
                close(STDOUT_FILENO);
                dup(stdout_fd[1]);
                close(stdout_fd[0]);
            }
            if(err) {
                close(STDERR_FILENO);
                dup(stderr_fd[1]);
                close(stderr_fd[0]);
            }
            execlp("su", "su", "-c", command, NULL);
            return false;

        default:
            instance->pid = child;
            if(in) {
                close(stdin_fd[0]);
                instance->stdin_fd = stdin_fd[1];
            }
            if(out) {
                close(stdout_fd[1]);
                instance->stdout_fd = stdout_fd[0];
                int r = fcntl(instance->stdout_fd, F_SETFL, O_NONBLOCK);
                libsu_LOG_INFO("libsu - fcntl returned: %d", r);
            }
            if(err) {
                close(stderr_fd[1]);
                instance->stderr_fd = stderr_fd[0];
                int r = fcntl(instance->stderr_fd, F_SETFL, O_NONBLOCK);
                libsu_LOG_INFO("libsu - fcntl returned: %d", r);
            }
#ifndef __ANDROID__
            // linux su requests a password, mimic this since stdout and stderr are being redirected
            // a new line will be entered by the user upon attempting to submit the password
            // so no need to explicitly print a new line
            printf("Password: ");
            fflush(stdout);
#endif
            int status;
            int r = waitpid(instance->pid, &status, 0);
            int errno_ = errno;
            const char * errnoString = strerror(errno);
            if (errno_ != 0) {
                libsu_LOG_ERROR("waitpid returned %d, errno: %d, errno string: %s\n", r, errno_, errnoString);
                return false;
            }

            libsu_read(instance);

            if (r != -1) {
                if (WIFEXITED(status)) {
                    instance->return_code = WEXITSTATUS(status);
                    instance->exited_normally = true;
                } else if (WIFSIGNALED(status)) {
                    instance->signal = -WTERMSIG(status);
                    instance->exited_from_signal = true;
                } else {
                    // third macro is WIFSTOPPED
                    // we should probably let the OS manage this

                    // Should never happen - waitpid(2) says
                    // "One of the first three macros will evaluate to a non-zero (true) value".
                }
            }
            return true;
    }
    return true;
}

void libsu_cleanup(libsu_processimage * instance) {
    if(instance->stdin_fd) {
        close(instance->stdin_fd);
        instance->stdin_fd = 0;
    }
    if(instance->stdout_fd) {
        close(instance->stdout_fd);
        instance->stdout_fd = 0;
        if (instance->stdout_should_be_freed) {
            free(instance->string_stdout);
            instance->string_stdout = NULL;
        }
    }
    if(instance->stderr_fd) {
        close(instance->stderr_fd);
        instance->stderr_fd = 0;
        if (instance->stderr_should_be_freed) {
            free(instance->string_stderr);
            instance->string_stderr = NULL;
        }
    }
    libsu_processimage_clear(instance);
}
