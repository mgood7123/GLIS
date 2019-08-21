//
// Created by konek on 8/19/2019.
//

#include "shm.h"
#include <sys/mman.h>

bool SHM_create(int & fd, void ** data, size_t size) {
    fd = ashmem_create_region("my_shm_region", size);
    if(fd < 0) {
        LOG_ERROR_SHM("ashmem_create_region: %s\n", strerror(errno));
        return false;
    }
    // Use fd to mmap from offset "0" to size mentioned below,
    *data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    return *data != MAP_FAILED;
}