// LINUX

#include <glis/glis.hpp>

int main() {
    GLIS glis;
    GLIS_CLASS glis_class;
    glis.GLIS_setupOnScreenRendering(glis_class);
    glis.GLIS_destroy_GLIS(glis_class);
    return 0;
}