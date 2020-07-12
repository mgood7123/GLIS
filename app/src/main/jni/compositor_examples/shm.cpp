//
// Created by konek on 8/20/2019.
//

#include <glis/glis.hpp>
#define LOG_TAG "shm"

GLIS glis;

int main() {
    glis.GLIS_INIT_SHARED_MEMORY(15, 15);
    LOG_INFO("creating window %d", 0);
    size_t win_id1 = glis.GLIS_new_window(0, 0, 5, 5);
    LOG_INFO("window id: %zu", win_id1);
    LOG_INFO("creating window %d", 1);
    size_t win_id2 = glis.GLIS_new_window(5, 5, 10, 10);
    LOG_INFO("window id: %zu", win_id2);
    LOG_INFO("creating window %d", 2);
    size_t win_id3 = glis.GLIS_new_window(10, 10, 15, 15);
    LOG_INFO("window id: %zu", win_id3);
    // TODO: example
    return 0;
}