//
// Created by konek on 8/20/2019.
//

#include <libsu.h>
#include <SysV.hpp>

int main() {
    bool hasRoot = libsu_has_root_access();
    libsu_LOG_INFO("libsu has root: %s", hasRoot ? "true" : "false");
    if (hasRoot) {
        // create once, use everywhere
        
        // no need to unmount when we are done
        
        // create tmp dir
        libsu_processimage instance;
        libsu_print_info(&instance, libsu_sudo(&instance, "mount -o remount,rw /"));
        libsu_cleanup(&instance);

        libsu_print_info(&instance, libsu_sudo(&instance, "mkdir /tmp"));
        libsu_cleanup(&instance);

        libsu_print_info(&instance, libsu_sudo(&instance, "chmod 777 /tmp"));
        libsu_cleanup(&instance);

        // a process can mount tmpfs
        // however due to mount namespaces, this will only be visible in the process that mounts it
        // this means it cannot be unmounted by another process
        // which means if a process that hosts the tmpfs dies,
        // while other processes are using that tmp,
        // it would become invalidated for all other processes and would probably cause them
        // all to crash since the shared memory is no longer mapped/shared

        // use global namespace, this requires magisk su

        libsu_print_info(&instance, libsu_sudo(&instance, true, "mount -t tmpfs -o size=512m tmpfs /tmp"));
        libsu_cleanup(&instance);

        libsu_print_info(&instance, libsu_sudo(&instance, "mount -o remount,ro /"));
        libsu_cleanup(&instance);

        shm_set_root_directory("/tmp");
        int r = shm_open("name", O_CREAT, 0666);
        LOG_INFO("shm_open returned %d", r);
        libsu_print_info(&instance, libsu_sudo(&instance, "ls -l /tmp/name"));
        libsu_cleanup(&instance);
        r = shm_unlink("name");
        LOG_INFO("shm_unlink returned %d", r);
        
        // no need to unmount and rmdir

        LOG_INFO("attempting to get killed by low memory killer");
        while(true) {
            LOG_INFO("allocating 1024");
            malloc(1024);
            sleep(1);
        }
    }
    return 0;
}
