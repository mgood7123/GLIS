#include "SysV.hpp"

int main() {
    const char * name = "test_shm";
    shm_open(name, O_CREAT, 0666);
    shm_unlink(name);
}
