#include "libsu.h"

int main() {
    bool hasRoot = libsu_has_root_access();
    libsu_LOG_INFO("libsu has root: %s", hasRoot ? "true" : "false");
    if (!hasRoot) return 0;

    libsu_processimage instance;
    bool r;
    r = libsu_sudo(&instance, "ls /");
    libsu_LOG_INFO("libsu returned: %s", r ? "true" : "false");
    libsu_LOG_INFO("libsu instance return code: %d", instance.return_code);
    libsu_LOG_INFO("libsu stdout: %s", instance.string_stdout);
    libsu_LOG_INFO("libsu stderr: %s", instance.string_stderr);
    libsu_cleanup(&instance);

    r = libsu_sudo(&instance, "true");
    libsu_LOG_INFO("libsu returned: %s", r ? "true" : "false");
    libsu_LOG_INFO("libsu instance return code: %d", instance.return_code);
    libsu_LOG_INFO("libsu stdout: %s", instance.string_stdout);
    libsu_LOG_INFO("libsu stderr: %s", instance.string_stderr);
    libsu_cleanup(&instance);
    return 0;
}