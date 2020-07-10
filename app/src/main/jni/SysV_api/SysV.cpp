#include "SysV.hpp"
#include <string>
#include <limits.h>
#include <sys/stat.h>
#include <unistd.h>

// https://pubs.opengroup.org/onlinepubs/9699919799/functions/shm_open.html
// https://pubs.opengroup.org/onlinepubs/9699919799/functions/shm_unlink.html

/*

case 1: shm_open;
case 2: shm_open; shm_unlink;
case 3: shm_open; mmap;
case 4: shm_open; mmap; munmap;
case 5: shm_open; mmap; shm_unlink;
case 6: shm_open; mmap; shm_unlink; munmap;
case 7: shm_open; mmap; munmap; shm_unlink;

on open():
    the file is created in /tmp/FILE_NAME
on mmap():
    a reference is associated with /tmp/FILE_NAME
on munmap():
    the reference to /tmp/FILE_NAME is removed
    if the file /tmp/FILE_NAME is queued for removal then
        the file /tmp/FILE_NAME is removed after the reference is removed
on unlink():
    the file /tmp/FILE_NAME is removed
    if the file /tmp/FILE_NAME is associated to a reference
        then the file /tmp/FILE_NAME is queued for removal
        upon the reference to /tmp/FILE_NAME being removed
*/

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

/*
If one or more references to the shared memory object exist when the object is unlinked, the name shall be removed before shm_unlink() returns, but the removal of the memory object contents shall be postponed until all open and map references to the shared memory object have been removed.

Even if the object continues to exist after the last shm_unlink(), reuse of the name shall subsequently cause shm_open() to behave as if no shared memory object of this name exists (that is, shm_open() will fail if O_CREAT is not set, or will create a new shared memory object if O_CREAT is set).
*/

int shm_unlink(const char *name) {
    std::string path = resolve_name(name);
    unlink(path.c_str());
    return 0;
}
