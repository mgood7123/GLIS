#include "libsu.h"

int main() {
    bool hasRoot = libsu_has_root_access();
    libsu_LOG_INFO("libsu has root: %s", hasRoot ? "true" : "false");
    if (!hasRoot) return 0;

    libsu_processimage instance;
    libsu_print_info(&instance, libsu_sudo(&instance, "ls -l /"));
    libsu_cleanup(&instance);

    libsu_print_info(&instance, libsu_sudo(&instance, "umount /var"));
    libsu_cleanup(&instance);
    return 0;
}