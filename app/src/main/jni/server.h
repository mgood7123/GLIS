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
    #define SERVER_LOG_INFO printf
    #define SERVER_LOG_ERROR printf
#else
    #include <strings.h>
    #include <android/log.h>

    #define SERVER_LOG_TAG "server_core"
    #define SERVER_LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO, SERVER_LOG_TAG, __VA_ARGS__)
    #define SERVER_LOG_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, SERVER_LOG_TAG, __VA_ARGS__)
#endif

#include "server_core.h"

#endif //GLNE_SERVER_H
