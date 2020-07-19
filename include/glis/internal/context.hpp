#include <glis/ipc/server_core.hpp>

#pragma once

class GLIS_CLASS {
public:
    int init_GLIS = false;
    bool
            init_eglGetDisplay = false,
            init_eglInitialize = false,
            init_eglChooseConfig = false,
            init_eglCreateWindowSurface = false,
            init_eglCreatePbufferSurface = false,
            init_eglCreateContext = false,
            init_eglMakeCurrent = false,
            init_debug = false;
    const GLint
            *configuration_attributes = nullptr,
            *context_attributes = nullptr,
            *surface_attributes = nullptr;
    EGLint
            eglMajVers = 0,
            eglMinVers = 0,
            number_of_configurations = 0;
    EGLNativeDisplayType display_id = EGL_DEFAULT_DISPLAY;
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLConfig configuration = 0;
    EGLContext
            context = EGL_NO_CONTEXT,
            shared_context = EGL_NO_CONTEXT;
    Magnum::Platform::GLContext * contextMagnum = nullptr;
    bool debug_context = false;
    EGLSurface surface = EGL_NO_SURFACE;
    // previously: ANativeWindow *native_window = nullptr;
    EGLNativeWindowType native_window = 0;
    GLint
            width = 0,
            height = 0;
    SOCKET_SERVER server;
    Kernel KERNEL;
};
