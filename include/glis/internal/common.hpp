//
// Created by konek on 8/20/2019.
//

#pragma once

#include <stdlib.h>
#include <assert.h>
#include <malloc.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <poll.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <Magnum/Platform/GLContext.h> // OpenGL
#include <Magnum/Platform/WindowlessEglApplication.h> // EGL
#include <Magnum/GL/Context.h> // OpenGL
#ifdef __ANDROID__
#include <jni.h>
#include <android/native_window.h> // requires ndk r5 or newer
#include <android/native_window_jni.h> // requires ndk r5 or newer
#endif
