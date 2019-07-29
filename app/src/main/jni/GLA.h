//
// Created by konek on 7/26/2019.
//

#ifndef GLNE_GLA_H
#define GLNE_GLA_H

#include <android/native_window.h> // requires ndk r5 or newer
#include <EGL/egl.h> // requires ndk r5 or newer
#include <GLES3/gl32.h>
#include "logger.h"

#define LOG_TAG "EglSample"

class GLIS_CLASS {
public:
    int init_GLIS = false;
    bool
            init_eglGetDisplay = false,
            init_eglInitialize = false,
            init_eglChooseConfig = false,
            init_eglCreateContext = false,
            init_eglCreateWindowSurface = false,
            init_eglCreatePbufferSurface = false,
            init_eglMakeCurrent = false;
    const GLint *configuration_attributes = nullptr, *context_attributes = nullptr, *surface_attributes = nullptr;
    EGLint eglMajVers = 0, eglMinVers = 0, number_of_configurations = 0;
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLConfig configuration = 0;
    EGLContext context = EGL_NO_CONTEXT, shared_context = EGL_NO_CONTEXT;
    EGLSurface surface = EGL_NO_SURFACE;
    ANativeWindow * native_window = nullptr;
    GLint width = 0, height = 0;
};

void GLIS_destroy_GLIS(class GLIS_CLASS & GLIS) {
    if (GLIS.init_eglMakeCurrent) {
        eglMakeCurrent(GLIS.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        GLIS.init_eglMakeCurrent = false;
    }
    if(GLIS.init_eglCreateContext) {
        eglDestroyContext(GLIS.display, GLIS.context);
        GLIS.context = EGL_NO_CONTEXT;
        GLIS.init_eglCreateContext = false;
    }
    if (GLIS.init_eglCreateWindowSurface || GLIS.init_eglCreatePbufferSurface) {
        eglDestroySurface(GLIS.display, GLIS.surface);
        GLIS.surface = EGL_NO_SURFACE;
        GLIS.init_eglCreateWindowSurface = false;
        GLIS.init_eglCreatePbufferSurface = false;
    }
    if (GLIS.init_eglInitialize) {
        eglTerminate(GLIS.display);
        GLIS.init_eglInitialize = false;
    }
    if (GLIS.init_eglGetDisplay) {
        GLIS.display = EGL_NO_DISPLAY;
        GLIS.init_eglGetDisplay = false;
    }
}


bool GLIS_init_display(class GLIS_CLASS & GLIS) {
    if ((GLIS.display = eglGetDisplay(EGL_DEFAULT_DISPLAY)) == EGL_NO_DISPLAY) {
        LOG_ERROR("eglGetDisplay() returned error %d", eglGetError());
        return false;
    }
    GLIS.init_eglGetDisplay = true;

    if (eglInitialize(GLIS.display, &GLIS.eglMajVers, &GLIS.eglMinVers) != EGL_TRUE) {
        LOG_ERROR("eglInitialize() returned error %d", eglGetError());
        return false;
    }
    GLIS.init_eglInitialize = true;
    
    LOG_INFO("EGL init with version %d.%d", GLIS.eglMajVers, GLIS.eglMinVers);
    return true;
}

bool GLIS_init_config(class GLIS_CLASS & GLIS) {
    if (!eglChooseConfig(GLIS.display, GLIS.configuration_attributes, &GLIS.configuration, 1, &GLIS.number_of_configurations)) {
        LOG_ERROR("eglChooseConfig() returned error %d", eglGetError());
        return false;
    }
    GLIS.init_eglChooseConfig = true;
    return true;
}

bool GLIS_create_context(class GLIS_CLASS & GLIS) {
    if (!(GLIS.context = eglCreateContext(GLIS.display, GLIS.configuration, GLIS.shared_context, GLIS.configuration_attributes))) {
        LOG_ERROR("eglCreateContext() returned error %d", eglGetError());
        return false;
    }
    GLIS.init_eglCreateContext = true;
    return true;
}

bool GLIS_init_surface_CreateWindowSurface(class GLIS_CLASS & GLIS) {
    if (!(GLIS.surface = eglCreateWindowSurface(GLIS.display, GLIS.configuration, GLIS.native_window, nullptr))) {
        LOG_ERROR("eglCreateWindowSurface() returned error %d", eglGetError());
        return false;
    }
    GLIS.init_eglCreateWindowSurface = true;
    return true;
}

bool GLIS_init_surface_CreatePbufferSurface(class GLIS_CLASS & GLIS) {
    if (!(GLIS.surface = eglCreatePbufferSurface(GLIS.display, GLIS.configuration, GLIS.surface_attributes))) {
        LOG_ERROR("eglCreatePbufferSurface() returned error %d", eglGetError());
        return false;
    }
    GLIS.init_eglCreatePbufferSurface = true;
    return true;
}

bool GLIS_switch_to_context(class GLIS_CLASS & GLIS) {
    if (!eglMakeCurrent(GLIS.display, GLIS.surface, GLIS.surface, GLIS.context)) {
        LOG_ERROR("eglMakeCurrent() returned error %d", eglGetError());
        return false;
    }
    GLIS.init_eglMakeCurrent = true;
    return true;
}

bool GLIS_get_width_height(class GLIS_CLASS & GLIS) {
    if (!eglQuerySurface(GLIS.display, GLIS.surface, EGL_WIDTH, &GLIS.width) ||
        !eglQuerySurface(GLIS.display, GLIS.surface, EGL_HEIGHT, &GLIS.height)) {
        LOG_ERROR("eglQuerySurface() returned error %d", eglGetError());
        return false;
    }
    return true;
}

bool GLIS_initialize(class GLIS_CLASS & GLIS) {
    LOG_INFO("Initializing");

    if (!GLIS_init_display(GLIS)) {
        GLIS_destroy_GLIS(GLIS);
        return false;
    }
    if (!GLIS_init_config(GLIS)) {
        GLIS_destroy_GLIS(GLIS);
        return false;
    }
    if (GLIS.surface_attributes != nullptr) {
        for (int i = 0; GLIS.surface_attributes[i] != EGL_NONE; i++) {
            if (GLIS.surface_attributes[i] == EGL_SURFACE_TYPE) {
                bool ret = false;
                if (GLIS.surface_attributes[i+1] == EGL_WINDOW_BIT)
                    ret = GLIS_init_surface_CreateWindowSurface(GLIS);
                else if (GLIS.surface_attributes[i+1] == EGL_PBUFFER_BIT)
                    ret = GLIS_init_surface_CreatePbufferSurface(GLIS);
                else {
                    LOG_INFO("unknown surface type: %d", GLIS.surface_attributes[i+1]);
                    return false;
                }
                if (!ret) {
                    GLIS_destroy_GLIS(GLIS);
                    return false;
                }
                break;
            }
        }
    }
    if (!GLIS_create_context(GLIS)) {
        GLIS_destroy_GLIS(GLIS);
        return false;
    }
    if (!GLIS_switch_to_context(GLIS)) {
        GLIS_destroy_GLIS(GLIS);
        return false;
    }
    if (!GLIS_get_width_height(GLIS)) {
        GLIS_destroy_GLIS(GLIS);
        return false;
    }

    LOG_INFO("Initialized");
    return true;
}

bool GLIS_setupOnScreenRendering(class GLIS_CLASS & GLIS) {
    const EGLint confAttr[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RED_SIZE,   8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE,  8,
            EGL_ALPHA_SIZE, 8,
            EGL_DEPTH_SIZE, 16,
            EGL_NONE
    };
    GLIS.configuration_attributes = confAttr;

    const EGLint ctxAttr[] = {
            EGL_CONTEXT_CLIENT_VERSION, 3,
            EGL_NONE
    };
    GLIS.context_attributes = ctxAttr;

    const EGLint surfaceAttr[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RED_SIZE,   8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE,  8,
            EGL_ALPHA_SIZE, 8,
            EGL_DEPTH_SIZE, 16,
            EGL_NONE
    };
    GLIS.surface_attributes = surfaceAttr;
    return GLIS_initialize(GLIS);
}

bool GLIS_setupOffScreenRendering(class GLIS_CLASS & GLIS, int w, int h) {
    const EGLint confAttr[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
            EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
            EGL_RED_SIZE,   8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE,  8,
            EGL_ALPHA_SIZE, 8,
            EGL_DEPTH_SIZE, 16,
            EGL_NONE
    };
    GLIS.configuration_attributes = confAttr;

    const EGLint ctxAttr[] = {
            EGL_CONTEXT_CLIENT_VERSION, 3,
            EGL_NONE
    };
    GLIS.context_attributes = ctxAttr;

    const EGLint surfaceAttr[] = {
             EGL_WIDTH, w,
             EGL_HEIGHT, h,
             EGL_NONE
    };
    GLIS.surface_attributes = surfaceAttr;
    return GLIS_initialize(GLIS);
}
#endif //GLNE_GLA_H
