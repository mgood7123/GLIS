//
// Copyright 2011 Tero Saarni
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// my current understanding of all this is that a compositor will render each application's frame buffer,
// and a window manager such as KDE or GNOME or I3,
// will work WITH the compositor retrieving information about windows and their position,
// then draw boarders around those windows and implement either stacking or tiling like functionality
// depending on the windowing system type and assumably send information back to the compositor
// such as updates on window changes
// for example if the window is minimized or its position changes,
// the compositor will then redraw itself as it sees fit according to the recieved information

#include <stdint.h>
#include <jni.h>
#include <android/native_window.h> // requires ndk r5 or newer
#include <android/native_window_jni.h> // requires ndk r5 or newer
#include <pthread.h>

#include "compositor.h"
#include "logger.h"
#include "GLA.h"
#include <vector>

#define LOG_TAG "EglSample"

struct GLINITIALIZATIONSTRUCTURE WM[2];

extern "C" JNIEXPORT void JNICALL Java_glnative_example_NativeView_nativeSetSurface(JNIEnv* jenv,
                                                                                    jclass type, jobject surface)
{
    if (surface != nullptr) {
        WM[0].native_window = ANativeWindow_fromSurface(jenv, surface);
        WM[1].native_window = ANativeWindow_fromSurface(jenv, surface);
        LOG_INFO("Got window %p", WM[0].native_window);
        LOG_INFO("Got window %p", WM[1].native_window);
    } else {
        LOG_INFO("Releasing window");
        ANativeWindow_release(WM[0].native_window);
        ANativeWindow_release(WM[1].native_window);
        WM[0].native_window = nullptr;
        WM[1].native_window = nullptr;
    }
}

void destroy(int index) {
    LOG_INFO("Destroying context");

    eglMakeCurrent(WM[index].display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(WM[index].display, WM[index].context);
    eglDestroySurface(WM[index].display, WM[index].surface);
    eglTerminate(WM[index].display);

    WM[index].display = EGL_NO_DISPLAY;
    WM[index].surface = EGL_NO_SURFACE;
    WM[index].context = EGL_NO_CONTEXT;
}


bool initialize(int index)
{
    const EGLint attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_NONE
    };
    WM[index].attrib_list = attribs;

    // to understand what components contribute to what parts of the API, we must first understand what it does
    // to understand what a function does, we must first read its documentation

    LOG_INFO("Initializing");

    if (!init_display(WM[index])) destroy(index);
    if (!init_config(WM[index])) destroy(index);
    if (!init_surface(WM[index])) destroy(index);
    if (!create_context(WM[index])) destroy(index);
    if (!switch_to_context(WM[index])) destroy(index);
    if (!get_width_height(WM[index])) destroy(index);

    LOG_INFO("Initialized");
    return true;
}

bool makeWindow(int index, int x, int y, size_t w, size_t h) {
    GLint X = static_cast<GLint>(x);
    GLint Y = static_cast<GLint>(y);
    GLsizei W = static_cast<GLsizei>(w);
    GLsizei H = static_cast<GLsizei>(h);
    GLuint frameBuffer;
    glGenFramebuffers(1, &frameBuffer);
    /* bind buffers */
    GLuint rboDepthStencil;
    glGenRenderbuffers(1, &rboDepthStencil);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepthStencil);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, W, H);
    GLenum enu = glCheckFramebufferStatus(1);
    LOG_INFO("enu: %d", enu);
    switch(enu) {
        case GL_FRAMEBUFFER_COMPLETE: {
            LOG_INFO("GL_FRAMEBUFFER_COMPLETE");
            break;
        }
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: {
            LOG_INFO("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
            break;
        }
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS: {
            LOG_INFO("GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS");
            break;
        }
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: {
            LOG_INFO("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
            break;
        }
        case GL_FRAMEBUFFER_UNSUPPORTED: {
            LOG_INFO("GL_FRAMEBUFFER_UNSUPPORTED");
            break;
        }
        case GL_INVALID_ENUM: {
            LOG_INFO("GL_INVALID_ENUM");
            break;
        }
        case 0: {
            GLenum en = glGetError();
            LOG_INFO("en: %d", en);
            switch (en) {
                case GL_NO_ERROR: {
                    LOG_INFO("GL_NO_ERROR");
                    break;
                }
                case GL_INVALID_ENUM: {
                    LOG_INFO("GL_INVALID_ENUM");
                    break;
                }
                case GL_INVALID_VALUE: {
                    LOG_INFO("GL_INVALID_VALUE");
                    break;
                }
                case GL_INVALID_OPERATION: {
                    LOG_INFO("GL_INVALID_OPERATION");
                    break;
                }
                case GL_INVALID_FRAMEBUFFER_OPERATION: {
                    LOG_INFO("GL_INVALID_FRAMEBUFFER_OPERATION");
                    break;
                }
                case GL_OUT_OF_MEMORY: {
                    LOG_INFO("GL_OUT_OF_MEMORY");
                    break;
                }
                default: {
                    LOG_INFO("unknown error: %d", en);
                    break;
                }
            }
            break;
        }
        default: {
            LOG_INFO("unknown return value: %d", enu);
            break;
        }
    }
//    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);
    glDeleteFramebuffers(1, &frameBuffer);
//    glEnable(GL_SCISSOR_TEST);
//    glScissor(X,Y,W,H);
    glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    if (!eglSwapBuffers(WM[index].display, WM[index].surface)) {
        LOG_ERROR("eglSwapBuffers() returned error %d", eglGetError());
    }
    return true;
}

struct window{
    int index;
    int x;
    int y;
    size_t w;
    size_t h;
};

void Xmain(struct window *window) {
    initialize(window->index); // initialization MUST occur in the thread
    makeWindow(window->index, window->x,window->y,window->w,window->h);
}

void * ptm(void * arg) {
    struct window * window = static_cast<struct window*>(arg);
    while (WM[window->index].native_window == nullptr) {}
    Xmain(window);
    destroy(window->index);
    return nullptr;
}


extern "C" JNIEXPORT void JNICALL Java_glnative_example_NativeView_nativeOnStart(JNIEnv* jenv,
                                                                                    jclass type)
{
    long _threadId;
    struct window * w1 = new struct window;
    *w1 = {0,0,0,500,500};
    struct window * w2 = new struct window;
    *w2 = {1,500,500,500,500};
//    pthread_create(&_threadId, nullptr, ptm, w1);
        pthread_create(&_threadId, nullptr, ptm, w2);
}
