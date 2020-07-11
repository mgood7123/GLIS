//
// Created by konek on 8/20/2019.
//

#include <libsu.h>
#include <SysV.hpp>

int main() {
    pid_t ppid = getppid();
    bool hasRoot = libsu_has_root_access();
    libsu_LOG_INFO("(ppid: %ld) libsu has root: %s", ppid, hasRoot ? "true" : "false");
    if (hasRoot) {
        // create once, use everywhere
        
        // no need to unmount when we are done
        
        // create tmp dir
        libsu_processimage instance;
        libsu_print_info(&instance, "mount -o remount,rw /");
        libsu_cleanup(&instance);

        libsu_print_info(&instance, "mkdir /tmp");
        libsu_cleanup(&instance);

        libsu_print_info(&instance, "chmod 777 /tmp");
        libsu_cleanup(&instance);

        // a process can mount tmpfs
        // however due to mount namespaces, this will only be visible in the process that mounts it
        // this means it cannot be unmounted by another process
        // which means if a process that hosts the tmpfs dies,
        // while other processes are using that tmp,
        // it would become invalidated for all other processes and would probably cause them
        // all to crash since the shared memory is no longer mapped/shared

        // use global namespace, this requires magisk su

        libsu_print_info(&instance, true, "mount -t tmpfs -o size=512m tmpfs /tmp");
        libsu_cleanup(&instance);

        libsu_print_info(&instance, "mount -o remount,ro /");
        libsu_cleanup(&instance);

        shm_set_root_directory("/tmp");
        int r = shm_open("name", O_CREAT, 0666);
        libsu_LOG_INFO("shm_open returned %d", r);
        libsu_print_info(&instance, "ls -l /tmp/name");
        libsu_cleanup(&instance);
        r = shm_unlink("name");
        libsu_LOG_INFO("shm_unlink returned %d", r);
        
        // no need to unmount and rmdir

        libsu_daemon();
        // even if re orphan the process,
        // force killing the original parent will kill the orphan as well
        /*
        dreamlte:/ # ps 26229 26087 26148 -O GID
        USER           PID  PPID     VSZ    RSS WCHAN            ADDR S      GID NAME
        u0_a203      26087  3962 5175452 136456 SyS_epoll+ 7592963d58 S    10203 glnative.example
        u0_a203      26148 26087       0      0 do_exit             0 Z    10203 [LowMemoryKiller]
        u0_a203      26229     1   16448    864 hrtimer_n+ 7da702cab8 S    10203 LowMemoryKillerTest
         */
        while(true) {
            libsu_LOG_ERROR("(ppid: %ld) I AM ALIVE", getppid());
            sleep(1);
        }
    }
    return 0;
}
