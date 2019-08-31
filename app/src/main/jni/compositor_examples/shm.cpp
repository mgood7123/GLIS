//
// Created by konek on 8/20/2019.
//

#include "../GLIS.h"
#include "../shm.h"
#define LOG_TAG "shm"

int main() {
    GLIS_INIT_SHARED_MEMORY();
    LOG_INFO("creating window %d", 0);
    size_t win_id1 = GLIS_new_window(0, 0, 5, 5);
    LOG_INFO("window id: %zu", win_id1);
    assert(win_id1 == 0);
    LOG_INFO("creating window %d", 1);
    size_t win_id2 = GLIS_new_window(5, 5, 10, 10);
    LOG_INFO("window id: %zu", win_id2);
    assert(win_id2 == 1);
    LOG_INFO("creating window %d", 2);
    size_t win_id3 = GLIS_new_window(10, 10, 15, 15);
    LOG_INFO("window id: %zu", win_id3);
    assert(win_id3 == 2);
    // TODO: example
    return 0;
}