//
// Created by konek on 8/13/2019.
//

#ifndef GLNE_SERVER_H
#define GLNE_SERVER_H

#include <cstdlib>
#include <cassert>
#include <malloc.h>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <cerrno>
#include <unistd.h>
#include <poll.h>
#include <pthread.h>

#ifndef __ANDROID__
#define LOG_INFO printf
    #define LOG_ERROR printf
#else
    #include <strings.h>
    #include <android/log.h>

    #ifndef LOG_INFO
        #define LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
    #endif
    #ifndef LOG_ERROR
        #define LOG_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
    #endif
#endif

#include "server_core.h"

#endif //GLNE_SERVER_H
