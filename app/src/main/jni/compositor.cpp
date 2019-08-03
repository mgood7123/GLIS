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

#include <cstdint>
#include <jni.h>
#include <android/native_window.h> // requires ndk r5 or newer
#include <android/native_window_jni.h> // requires ndk r5 or newer
#include <pthread.h>

#include "compositor.h"
#include "logger.h"
#include "GLIS.h"
#include <vector>

#define LOG_TAG "EglSample"

class GLIS_CLASS CompositorMain;
class GLIS_CLASS Compositor[2];

extern "C" JNIEXPORT void JNICALL Java_glnative_example_NativeView_nativeSetSurface(JNIEnv* jenv,
                                                                                    jclass type,
                                                                                    jobject surface)
{
    if (surface != nullptr) {
        CompositorMain.native_window = ANativeWindow_fromSurface(jenv, surface);
        LOG_INFO("Got window %p", CompositorMain.native_window);
    } else {
        LOG_INFO("Releasing window");
        ANativeWindow_release(CompositorMain.native_window);
        CompositorMain.native_window = nullptr;
    }
}

bool makeWindow(int index, int x, int y, int w, int h) {
    auto X = static_cast<GLint>(x);
    auto Y = static_cast<GLint>(y);
    auto W = static_cast<GLsizei>(w);
    auto H = static_cast<GLsizei>(h);
    glEnable(GL_SCISSOR_TEST);
    glScissor(X,Y,W,H);
    glClearColor(0.0F, 1.0F, 1.0F, 1.0F);
    glClear(GL_COLOR_BUFFER_BIT);
    if (!eglSwapBuffers(Compositor[index].display, Compositor[index].surface)) {
        LOG_ERROR("eglSwapBuffers() returned error %d", eglGetError());
    }
    return true;
}

struct window{
    int index;
    int x;
    int y;
    int w;
    int h;
    EGLContext MainContext;
};

void Xmain(struct window *window) {
    if (GLIS_setupOnScreenRendering(Compositor[window->index], window->MainContext)) {
//    if (GLIS_setupOffScreenRendering(Compositor[window->index], window->w, window->h, window->MainContext)) {
        GLIS_error_to_string();
        // TODO: Xorg uses Textures to render, specifically Xorg renders FROM textures and DOES NOT modify them

        // create a new frame buffer
        GLuint FBOID;
        GLIS_error_to_string_exec(glGenFramebuffers(1, &FBOID));
        GLIS_error_to_string_exec(glBindFramebuffer(GL_FRAMEBUFFER, FBOID));
        GLuint rboColorId;
        GLIS_error_to_string_exec(glGenRenderbuffers(1, &rboColorId));
        GLIS_error_to_string_exec(glBindRenderbuffer(GL_RENDERBUFFER, rboColorId));
        GLIS_error_to_string_exec(glRenderbufferStorage(GL_RENDERBUFFER,
                                                        GL_RGB8,
                                                        Compositor[window->index].width,
                                                        Compositor[window->index].height));
        GLIS_error_to_string_exec(glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                                            GL_COLOR_ATTACHMENT0,
                                                            GL_RENDERBUFFER,
                                                            rboColorId));
        GLIS_error_to_string_exec(glBindFramebuffer(GL_FRAMEBUFFER, FBOID));

        GLenum FramebufferStatus = GLIS_error_to_string_exec(
            glCheckFramebufferStatus(GL_FRAMEBUFFER));

        if (FramebufferStatus != GL_FRAMEBUFFER_COMPLETE)
            LOG_ERROR("framebuffer is not complete");
        else
            LOG_INFO("framebuffer is complete");

        // bind system framebuffer
        GLIS_error_to_string_exec(glBindFramebuffer(GL_FRAMEBUFFER, 0));

        FramebufferStatus = GLIS_error_to_string_exec(
            glCheckFramebufferStatus(GL_FRAMEBUFFER));

        if (FramebufferStatus != GL_FRAMEBUFFER_COMPLETE)
            LOG_ERROR("framebuffer is not complete");
        else
            LOG_INFO("framebuffer is complete");

        // clear framebuffer
        GLIS_error_to_string_exec(glClearColor(0.0F, 1.0F, 1.0F, 1.0F));
        GLIS_error_to_string_exec(glClear(GL_COLOR_BUFFER_BIT));


        // copy FBO to FBO
        GLIS_error_to_string_exec(glBindFramebuffer(GL_READ_FRAMEBUFFER, FBOID));
        GLIS_error_to_string_exec(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));

        if (FramebufferStatus != GL_FRAMEBUFFER_COMPLETE)
            LOG_ERROR("framebuffer is not complete");
        else
            LOG_INFO("framebuffer is complete");

        GLIS_error_to_string_exec(glBlitFramebuffer(window->x, window->y, window->w + window->x, window->h + window->y,             // src rect
                                                    window->x, window->y, window->w + window->x, window->h + window->y,             // dst rect
                                                    GL_COLOR_BUFFER_BIT,        // buffer mask
                                                    GL_NEAREST));               // scale filter

        if (FramebufferStatus != GL_FRAMEBUFFER_COMPLETE)
            LOG_ERROR("framebuffer is not complete");
        else
            LOG_INFO("framebuffer is complete");

        // display system framebuffer
        if (!eglSwapBuffers(Compositor[window->index].display,
                            Compositor[window->index].surface)) {
            LOG_ERROR("eglSwapBuffers() returned error %d", eglGetError());
        }
    }
}

void * ptm(void * arg) {
    auto * window = static_cast<struct window*>(arg);
    Xmain(window);
    GLIS_destroy_GLIS(Compositor[window->index]);
    return nullptr;
}

void * COMPOSITORMAIN(void * arg) {
    LOG_INFO("waiting for main Compositor to obtain a native window");
    while (CompositorMain.native_window == nullptr) {}
    Compositor[1].native_window = CompositorMain.native_window;
    LOG_INFO("main Compositor has obtained a native window");
    LOG_INFO("initializing main Compositor");
//    if (GLIS_setupOnScreenRendering(CompositorMain)) {
        LOG_INFO("initialized main Compositor");
        long _threadId1;
        long _threadId2;
//        struct window *w1 = new struct window;
//        *w1 = {0, 0, 0, 500, 500};
        auto *w2 = new struct window;
        *w2 = {1, 500, 500, 500, 500, CompositorMain.context};
//        pthread_create(&_threadId1, nullptr, ptm, w1);
        LOG_INFO("starting test application");
        pthread_create(&_threadId2, nullptr, ptm, w2);
//    } else LOG_ERROR("failed to initialize main Compositor");
    return nullptr;
}


extern "C" JNIEXPORT void JNICALL Java_glnative_example_NativeView_nativeOnStart(JNIEnv* jenv,
                                                                                    jclass type)
{
    long _threadId;
    LOG_INFO("starting main Compositor");
    pthread_create(&_threadId, nullptr, COMPOSITORMAIN, nullptr);
}
