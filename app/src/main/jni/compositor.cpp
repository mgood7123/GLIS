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
// the compositor will then redraw itself as it sees fit according to the received information

// X uses a buffer to draw from, and this buffer must have a supported pixel format,
// and applications which write to this buffer must obtain a handle to this buffer,
// then convert their pixel data to match the buffer's pixel format then write to buffer

//  A client can choose to use any pixel format the server understand.
//  Buffer allocation is nowadays done by the client, and handle passed to the server,
//  but the opposite also exists.

// all of this is fundamentally incompatible with networking, because it relies on shared memory

// in networking case the pixel data is sent to the server, along with control messages,
// in shared memory case it is stored directly in the buffer,
// and control messages get sent to the server from the client

#include <glis/glis.hpp>

#define LOG_TAG "EglSample"

GLIS_CLASS CompositorMain;
GLIS glis;
GLIS_FONT font;
GLIS_FPS fps;

char *executableDir;
char *nativeLibsDir;

GLIS_COMPOSITOR_LOOP_FUNCTION(loop, glis, CompositorMain, font, fps, render, resize, close_) {
    glis.runUntilAndroidWindowClose(glis, CompositorMain, font, fps, render, resize, close_);
}

/*
[22:30] <emersion> 1. client renders using OpenGL
[22:30] <emersion> 2. OpenGL implementation sends the GPU buffer to the server, e.glis. via the linux-dmabuf protocol
[22:30] <emersion> 3. server receives it and imports it via OpenGL
[22:31] <emersion> 4. server composites the client using OpenGL, e.glis. on a buffer allocated via GBM
[22:31] <emersion> 5. server uses DRM to display the GBM buffer
 */

int COMPOSITORMAIN__() {
    LOG_INFO("called COMPOSITORMAIN__()");
    setenv("LD_LIBRARY_PATH", nativeLibsDir, 1);
    system(std::string(std::string("chmod -R 777 ") + executableDir).c_str());
    char *exe =
        const_cast<char *>(std::string(
            std::string(executableDir) + "/Arch/arm64-v8a/DefaultFramebuffer").c_str());
    char *args1[2] = {exe, 0};
    glis.GLIS_FORK(exe, args1);

    char *exe2 =
        const_cast<char *>(std::string(
            std::string(executableDir) + "/Arch/arm64-v8a/MovingWindowsB").c_str());
    char *args2[2] = {exe2, 0};
    glis.GLIS_FORK(exe2, args2);

    GLIS_COMPOSITOR_BEFORE_REQUEST_STARTUP(glis);
    GLIS_COMPOSITOR_HANDLE_STARTUP_REQUEST(glis);
    std::string f = std::string(executableDir) + "/fonts/Vera.ttf";
    GLIS_COMPOSITOR_DO_MAIN(
            glis,
            CompositorMain,
            fps,
            font,
            f.c_str(),
            128,
            loop,
            GLIS_COMPOSITOR_DEFAULT_DRAW_FUNCTION,
            GLIS_COMPOSITOR_DEFAULT_RESIZE_FUNCTION,
            GLIS_COMPOSITOR_DEFAULT_CLOSE_FUNCTION
    );
    return 0;
}

void * COMPOSITORMAIN(void * arg) {
    int * ret = new int;
    LOG_INFO("calling COMPOSITORMAIN__()");
    *ret = COMPOSITORMAIN__();
    return ret;
}

extern "C" JNIEXPORT void JNICALL Java_glnative_example_NativeView_nativeSetSurface(JNIEnv* jenv,
                                                                                    jobject type,
                                                                                    jobject surface,
                                                                                    jint w, jint h)
{
    if (surface != nullptr) {
        glis.getAndroidWindow(jenv, surface, CompositorMain, w, h);
        GLIS_COMPOSITOR_REQUEST_STARTUP(glis);
    } else {
        GLIS_COMPOSITOR_REQUEST_SHUTDOWN_MT(glis, CompositorMain);
        glis.destroyAndroidWindow(CompositorMain);
        CompositorMain.native_window = nullptr;
    }
}

long COMPOSITORMAIN_threadId;
extern "C" JNIEXPORT void JNICALL Java_glnative_example_NativeView_nativeOnStart(
        JNIEnv* jenv, jobject type, jstring ExecutablesDir, jstring NativeLibsDir
) {
    jboolean val;
    const char *a = jenv->GetStringUTFChars(ExecutablesDir, &val);
    size_t len = (strlen(a) + 1) * sizeof(char);
    executableDir = static_cast<char *>(malloc(len));
    memcpy(executableDir, a, len);
    jenv->ReleaseStringUTFChars(ExecutablesDir, a);
    const char *b = jenv->GetStringUTFChars(NativeLibsDir, &val);
    size_t lenb = (strlen(b) + 1) * sizeof(char);
    nativeLibsDir = static_cast<char *>(malloc(lenb));
    memcpy(nativeLibsDir, b, lenb);
    jenv->ReleaseStringUTFChars(NativeLibsDir, a);
    LOG_INFO("starting main Compositor");
    int e = pthread_create(&COMPOSITORMAIN_threadId, nullptr, COMPOSITORMAIN, nullptr);
    if (e != 0)
        LOG_ERROR("pthread_create(): errno: %d (%s) | return: %d (%s)", errno, strerror(errno), e,
                  strerror(e));
    else
        LOG_INFO("Compositor thread successfully started");
}

extern "C" JNIEXPORT void JNICALL Java_glnative_example_NativeView_nativeOnStop(JNIEnv* jenv,
                                                                                jobject type) {
    LOG_INFO("waiting for main Compositor to stop");
    int * ret;
    int e = pthread_join(COMPOSITORMAIN_threadId, reinterpret_cast<void **>(&ret));
    if (e != 0)
        LOG_ERROR("pthread_join(): errno: %d (%s) | return: %d (%s)", errno, strerror(errno), e,
                  strerror(e));
    else
        LOG_INFO("main Compositor has stopped: return code: %d", *ret);
    delete ret;
}