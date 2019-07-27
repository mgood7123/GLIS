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

#include "jniapi.h"
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

    glDisable(GL_DITHER);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    return true;
}

bool makeWindow(int index, int x, int y, size_t w, size_t h) {
    // glScissorArray and glViewportArray
    glScissor(static_cast<GLint>(x),static_cast<GLint>(y), static_cast<GLsizei>(w),static_cast<GLsizei>(h));
    glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
    glViewport(static_cast<GLint>(x), static_cast<GLint>(y), static_cast<GLsizei>(w), static_cast<GLsizei>(h));

    GLfloat ratio;
    ratio = (GLfloat) WM[index].width / WM[index].height;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustumf(-ratio, ratio, -1, 1, 1, 10);
    return true;
}

static GLint vertices[][3] = {
        { -0x10000, -0x10000, -0x10000 },
        {  0x10000, -0x10000, -0x10000 },
        {  0x10000,  0x10000, -0x10000 },
        { -0x10000,  0x10000, -0x10000 },
        { -0x10000, -0x10000,  0x10000 },
        {  0x10000, -0x10000,  0x10000 },
        {  0x10000,  0x10000,  0x10000 },
        { -0x10000,  0x10000,  0x10000 }
};

static GLint colors[][4] = {
        { 0x00000, 0x00000, 0x00000, 0x10000 },
        { 0x10000, 0x00000, 0x00000, 0x10000 },
        { 0x10000, 0x10000, 0x00000, 0x10000 },
        { 0x00000, 0x10000, 0x00000, 0x10000 },
        { 0x00000, 0x00000, 0x10000, 0x10000 },
        { 0x10000, 0x00000, 0x10000, 0x10000 },
        { 0x10000, 0x10000, 0x10000, 0x10000 },
        { 0x00000, 0x10000, 0x10000, 0x10000 }
};

GLubyte indices[] = {
        0, 4, 5,    0, 5, 1,
        1, 5, 6,    1, 6, 2,
        2, 6, 7,    2, 7, 3,
        3, 7, 4,    3, 4, 0,
        4, 7, 6,    4, 6, 5,
        3, 0, 1,    3, 1, 2
};

GLfloat _angle;

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
    while(WM[window->index].native_window != nullptr) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0.0f, 0.0f, -3.0f);
        glRotatef(_angle, 0.0f, 1.0f, 0.0f);
        glRotatef(_angle * 0.25f, 1.0f, 0.0f, 0.0f);

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);

        glFrontFace(GL_CW);
        glVertexPointer(3, GL_FIXED, 0, vertices);
        glColorPointer(4, GL_FIXED, 0, colors);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, indices);

        _angle += 1.2f;
        if (!eglSwapBuffers(WM[window->index].display, WM[window->index].surface)) { // this WILL NOT support multiple windows
            LOG_ERROR("eglSwapBuffers() returned error %d", eglGetError());
        }
    }
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
    pthread_create(&_threadId, nullptr, ptm, w1);
    //    pthread_create(&_threadId, nullptr, ptm, w2);
}
