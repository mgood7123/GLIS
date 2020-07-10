#include <sys/types.h>
#include <fcntl.h>

void shm_set_root_directory(const char *path);

int shm_open(const char *name, int oflag, mode_t mode);
int shm_unlink(const char *name);
