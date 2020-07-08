//
// Created by smallville7123 on 8/07/20.
//

#ifndef ANDROIDCOMPOSITOR_LIBSU_H
#define ANDROIDCOMPOSITOR_LIBSU_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct {
    pid_t pid;
    int infd,
            outfd,
            errfd;
} libsu_processimage;

void
libsu_mkprocess(const char *cmd, libsu_processimage *ret, const char in, const char out, const char err) {
    int infd[2], outfd[2], errfd[2];
    pid_t child;

    if(in)
        if(pipe(infd))
            return;
    if(out)
        if(pipe(outfd))
            return;
    if(err)
        if(pipe(errfd))
            return;

    switch((child = fork())) {
        case -1:
            return;

        case 0:
            if(in) {
                close(STDIN_FILENO);
                dup(infd[0]);
                close(infd[1]);
            }
            if(out) {
                close(STDOUT_FILENO);
                dup(outfd[1]);
                close(outfd[0]);
            }
            if(err) {
                close(STDERR_FILENO);
                dup(errfd[1]);
                close(errfd[0]);
            }
            execlp(cmd, cmd, NULL);
            return;

        default:
            ret->pid = child;
            if(in) {
                close(infd[0]);
                ret->infd = infd[1];
            }
            if(out) {
                close(outfd[1]);
                ret->outfd = outfd[0];
            }
            if(err) {
                close(errfd[1]);
                ret->errfd = errfd[0];
            }
    }
    return;
}

void
libsu_rmprocess(libsu_processimage *pimage) {
    if(pimage->infd) {
        close(pimage->infd);
        pimage->infd = 0;
    }
    if(pimage->outfd) {
        close(pimage->outfd);
        pimage->outfd = 0;
    }
    if(pimage->errfd) {
        close(pimage->errfd);
        pimage->errfd = 0;
    }
    waitpid(pimage->pid, NULL, 0);
    pimage->pid = 0;
}

libsu_processimage libsu_process;

int
libsu_system(const char *cmd) {
    int ret = -1;
    pid_t child;

    switch((child = fork())) {
        case -1:
            return -1;

        case 0:
            return execlp("su", "su", "-c", cmd, NULL);

        default:
            waitpid(child, &ret, 0);
    }

    return ret;
}

bool libsu_sudo(const char * cmd) {
    size_t len = strlen(cmd);
    bool ret = true;
    if(libsu_process.pid) {
        if (write(libsu_process.infd, cmd, len) != len) ret = false;
        if (write(libsu_process.infd, "\n", 1) != 1) ret = false;
    } else if(libsu_system(cmd) < 0) ret = false;
    return ret;
}

char * libsu_get_stdout() {
//    libsu_process.outfd
}

void libsu_startshell() {
    if(!libsu_process.pid)
        libsu_mkprocess("su", &libsu_process, 1, 0, 0);
}

void libsu_closeshell() {
    if(libsu_process.pid) {
        libsu_rmprocess(&libsu_process);
    }
}

#endif //ANDROIDCOMPOSITOR_LIBSU_H
