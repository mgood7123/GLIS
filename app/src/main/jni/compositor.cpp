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

int METHOD = 1;

void Xmain(struct window *window) {
    if (GLIS_setupOnScreenRendering(Compositor[window->index], window->MainContext)) {
        GLIS_error_to_string();
        // TODO: Xorg uses Textures to render, specifically Xorg renders FROM textures and DOES NOT modify them
        if (METHOD == 2) {
            GLuint frameBuffer;
            GLIS_error_to_string_exec(glGenFramebuffers(1, &frameBuffer));
            GLIS_error_to_string_exec(glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer));

            GLuint rboDepthStencil;
            GLIS_error_to_string_exec(glGenRenderbuffers(1, &rboDepthStencil));
            GLIS_error_to_string_exec(glBindRenderbuffer(GL_RENDERBUFFER, rboDepthStencil));
            GLIS_error_to_string_exec(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, window->w, window->h));

            GLIS_error_to_string_exec(glFramebufferRenderbuffer(
                    GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepthStencil
            ));

            GLIS_error_to_string_exec(glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer));

            GLenum FramebufferStatus = GLIS_error_to_string_exec(glCheckFramebufferStatus(GL_FRAMEBUFFER));

            if (FramebufferStatus != GL_FRAMEBUFFER_COMPLETE)
                LOG_ERROR("framebuffer is not complete");
            else {
                LOG_INFO("framebuffer is complete");
                GLIS_error_to_string_exec(glEnable(GL_SCISSOR_TEST));
                GLIS_error_to_string_exec(glScissor(window->x, window->y, window->w, window->h));
                GLIS_error_to_string_exec(glClearColor(0.0F, 1.0F, 1.0F, 1.0F));
                GLIS_error_to_string_exec(glClear(GL_COLOR_BUFFER_BIT));
//            makeWindow(window->index, window->x, window->y, window->w, window->h);
            }
        } else if (METHOD == 1) {
            GLuint vbo; // VBO
            float vertices[] = {
                    0.0F, 0.5F, // Vertex 1 (X, Y)
                    0.5F, -0.5F, // Vertex 2 (X, Y)
                    -0.5F, -0.5F  // Vertex 3 (X, Y)
            };
            GLuint vertexShader;
            GLuint fragmentShader;
            GLuint tex; // TEXTURE
            // VBO
            {
                GLIS_error_to_string_exec(glGenBuffers(1, &vbo)); // Generate 1 buffer
                GLIS_error_to_string_exec(glBindBuffer(GL_ARRAY_BUFFER, vbo));
                GLIS_error_to_string_exec(
                        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
            }
            // SHADER
            {
                const char *vertexSource = R"glsl( #version 320 es
in vec2 position;
void main()
{
    gl_Position = vec4(position, 0.0, 1.0);
} )glsl";

                const char *fragmentSource = R"glsl( #version 320 es
out lowp vec4 outColor;
uniform sampler2D tex;
void main()
{
    outColor = vec4(1.0, 1.0, 1.0, 1.0);

} )glsl";

                vertexShader = GLIS_createShader(GL_VERTEX_SHADER, vertexSource);
                fragmentShader = GLIS_createShader(GL_FRAGMENT_SHADER, fragmentSource);
                LOG_INFO("Creating Shader program");
                GLuint shaderProgram = GLIS_error_to_string_exec(glCreateProgram());
                LOG_INFO("Attaching vertex Shader to program");
                GLIS_error_to_string_exec(glAttachShader(shaderProgram, vertexShader));
                LOG_INFO("Attaching fragment Shader to program");
                GLIS_error_to_string_exec(glAttachShader(shaderProgram, fragmentShader));
                LOG_INFO("Linking Shader program");
                GLIS_error_to_string_exec(glLinkProgram(shaderProgram));
                LOG_INFO("Validating Shader program");
                GLboolean ProgramIsValid = GLIS_error_to_string_exec(GLIS_validate_program(shaderProgram));
                assert(ProgramIsValid == GL_TRUE);
                LOG_INFO("Using Shader program");
                GLIS_error_to_string_exec(glUseProgram(shaderProgram));
                GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
                GLIS_error_to_string_exec(
                        glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, nullptr));
                GLIS_error_to_string_exec(glEnableVertexAttribArray(posAttrib));
                GLIS_error_to_string_exec(glDrawArrays(GL_TRIANGLES, 0, 3));
            }
            // TEXTURE
            {
                GLIS_error_to_string_exec(glGenTextures(1, &tex));
                GLIS_error_to_string_exec(glBindTexture(GL_TEXTURE_2D, tex));
                // wrapping
                GLIS_error_to_string_exec(
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
                GLIS_error_to_string_exec(
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
                // filtering
                GLIS_error_to_string_exec(
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
                GLIS_error_to_string_exec(
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
                // load texture
                // Black/white checkerboard
                float pixels[] = {
                        0.0F, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F,
                        1.0F, 1.0F, 1.0F, 0.0F, 0.0F, 0.0F
                };
                GLIS_error_to_string_exec(
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 2, 2, 0, GL_RGBA, GL_FLOAT, pixels)
                );
                GLIS_error_to_string_exec(
                    eglSwapBuffers(
                        Compositor[window->index].display,
                        Compositor[window->index].surface
                    )
                );
            }
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
