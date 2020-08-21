//
// Created by konek on 8/14/2019.
//

#include <glis/glis.hpp>

GLIS_CLASS G;
GLIS glis;

int main() {
    glis.GLIS_INIT_SHARED_MEMORY(1080,720);
    for (int i = 0; i < 100; i++) glis.GLIS_new_window(0, 0, 1080,720);
    return 0;
}