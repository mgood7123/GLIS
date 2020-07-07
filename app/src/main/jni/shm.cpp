//
// Created by konek on 8/19/2019.
//

#include "shm.h"
#include <sys/mman.h>

char *SHM_str_humanise_bytes(off_t bytes) {
    char *data = new char[1024];
    if (bytes > 1 << 30) {
        sprintf(data, "%u.%2.2u Gigabytes",
                (int) (bytes >> 30),
                (int) (bytes & ((1 << 30) - 1)) / 10737419);
    } else if (bytes > 1 << 20) {
        int x = bytes + 5243;  /* for rounding */
        sprintf(data, "%u.%2.2u Megabytes",
                x >> 20, ((x & ((1 << 20) - 1)) * 100) >> 20);
    } else if (bytes > 1 << 10) {
        int x = bytes + 5;  /* for rounding */
        sprintf(data, "%u.%2.2u Kilobytes",
                x >> 10, ((x & ((1 << 10) - 1)) * 100) >> 10);
    } else {
        sprintf(data, "%u bytes", (int) bytes);
    }
    return data;
}

bool SHM_create(int &fd, int8_t **data, size_t size) {
    char *b = SHM_str_humanise_bytes(static_cast<off_t>(size));
    LOG_INFO_SHM("requesting %zu bytes (%s) of memory", size, b);
    free(b);
    fd = ashmem_create_region("my_shm_region", size);
    if(fd < 0) {
        LOG_ERROR_SHM("ashmem_create_region: errno: %d (%s)", errno, strerror(errno));
        return false;
    }

    if (!ashmem_valid(fd)) {
        LOG_ERROR_SHM("ashmem_valid: errno: %d (%s)", errno, strerror(errno));
        return false;
    }
    LOG_INFO_SHM("fd %d is valid and should be able to be opened", fd);
    int region_size = ashmem_get_size_region(fd);
    LOG_INFO_SHM("region created with %zu size", region_size);
    assert(region_size == size);

    // Use fd to mmap from offset "0" to size mentioned below,
    *data = static_cast<int8_t *>(mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
    if (*data == MAP_FAILED) {
        LOG_ERROR_SHM("mmap: errno: %d (%s)", errno, strerror(errno));
        return false;
    }
    return true;
}

bool SHM_resize(int &fd, int8_t **data, size_t size) {
    int ret = TEMP_FAILURE_RETRY(ioctl(fd, ASHMEM_SET_SIZE, size));
    if (ret < 0) {
        LOG_ERROR_SHM("ioctl: errno: %d (%s)", errno, strerror(errno));
        return false;
    }
    *data = static_cast<int8_t *>(mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
    if (*data == MAP_FAILED) {
        LOG_ERROR_SHM("mmap: errno: %d (%s)", errno, strerror(errno));
        return false;
    }
    return true;
}

bool SHM_close(int &fd) {
    int ret = close(fd);
    if (ret < 0) {
        LOG_ERROR_SHM("close: errno: %d (%s)", errno, strerror(errno));
        return false;
    }
    return true;
}

bool SHM_open(int &fd, int8_t **data, size_t size) {
    // fd and size should be sent from the process calling SHM_create, to the process calling SHM_open
    // Use fd to mmap from offset "0" to size mentioned below,
    if (!ashmem_valid(fd)) {
        LOG_ERROR_SHM("ashmem_valid: errno: %d (%s)", errno, strerror(errno));
        return false;
    }
    *data = static_cast<int8_t *>(mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
    if (*data == MAP_FAILED) {
        LOG_ERROR_SHM("mmap: errno: %d (%s)", errno, strerror(errno));
        return false;
    }
    return true;
}