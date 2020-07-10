#include "SysV.hpp"
#include <string>
#include <limits.h>
#include <sys/stat.h>
#include <unistd.h>

// https://pubs.opengroup.org/onlinepubs/9699919799/functions/shm_open.html

char * shm_root_directory = const_cast<char*>("/tmp");

void shm_set_root_directory(const char *path) {
    shm_root_directory = const_cast<char*>(path);
}

std::string resolve_name(const char * name) {
    std::string path = shm_root_directory;
    path.append("/");
    path.append(name);
    return path;
}    

int shm_open(const char *name, int oflag, mode_t mode) {
    std::string path = resolve_name(name);
    int fd = open(path.c_str(), oflag, mode);
    return fd;
}

int shm_unlink(const char *name) {
    std::string path = resolve_name(name);
    unlink(path.c_str());
    return 0;
}
